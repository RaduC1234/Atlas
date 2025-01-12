#pragma once

#include <Atlas.hpp>
#include <cpr/cpr.h>
#include <nlohmann/json.hpp>
#include <thread>
#include <future>
#include <atomic>
#include <mutex>
#include "renderer/Color.hpp"
#include "window/Keyboard.hpp"

class NetworkSystem {
public:
    void update(float deltaTime, entt::registry &registry, uint64_t playerId) {
        if (!isSyncing.exchange(true)) {
            // Prevent multiple requests
            std::async(std::launch::async, &NetworkSystem::syncEntities, this, std::ref(registry), playerId);
        }
    }

private:
    std::atomic<bool> isSyncing{false}; // Prevent duplicate requests
    std::mutex registryMutex; // Ensure safe registry modification

    void syncEntities(entt::registry &registry, uint64_t playerId) {
        nlohmann::json input;

        input["moveForward"] = Keyboard::isKeyPressed(Keyboard::W);
        input["moveBackwards"] = Keyboard::isKeyPressed(Keyboard::S);
        input["moveLeft"] = Keyboard::isKeyPressed(Keyboard::A);
        input["moveRight"] = Keyboard::isKeyPressed(Keyboard::D);

        auto response = cpr::Post(
            cpr::Url{"http://localhost:8080/sync_entities"},
            cpr::Body(nlohmann::json{
                {"playerId", playerId},
                {"input", input}
            }.dump()),

            cpr::Header{{"Content-Type", "application/json"}}
        );

        if (response.status_code != 200) {
            std::cerr << "Sync failed: " << response.status_code << std::endl;
            isSyncing = false; // Reset flag
            return;
        }

        try {
            auto jsonResponse = nlohmann::json::parse(response.text);
            overwriteRegistry(jsonResponse, registry, playerId);
        } catch (const std::exception &e) {
            std::cerr << "Error parsing sync response: " << e.what() << std::endl;
        }

        isSyncing = false; // Reset flag after completion
    }

    void overwriteRegistry(const nlohmann::json &jsonResponse, entt::registry &registry, uint64_t playerId) {
        registry.clear(); // Clear all entities

        for (const auto &entityData: jsonResponse["entities"]) {
            auto entity = registry.create();
            auto type = static_cast<EntityType>(entityData["entityType"].get<int>());
            auto tileCode = entityData["tile-code"].get<uint32_t>();
            std::string textureName = "";

            if (tileCode >= EntityCode::TILE_CODE && tileCode < EntityCode::TILE_CODE + EntityCode::NEXT) {
                std::ostringstream oss;
                oss << "tile_" << std::setw(4) << std::setfill('0') << (tileCode % 10000);
                textureName = oss.str();
            }

            registry.emplace<NetworkComponent>(
                entity,
                entityData["networkId"].get<uint64_t>(),
                type
            );

            switch (type) {
                case PAWN: {
                    bool forward = entityData["PawnComponent"]["moveForward"];
                    bool backwards = entityData["PawnComponent"]["moveBackwards"];
                    bool left = entityData["PawnComponent"]["moveLeft"];
                    bool right = entityData["PawnComponent"]["moveRight"];
                    registry.emplace<PawnComponent>(entity, entityData["PawnComponent"]["playerId"], forward, backwards, left, right, 0.0f);

                    if (!forward && !backwards && !left && !right) {
                        textureName = "front1";
                    }
                }
                case STATIC: {
                    registry.emplace<TransformComponent>(
                        entity,
                        glm::vec3(entityData["TransformComponent"]["position"][0],
                                  entityData["TransformComponent"]["position"][1],
                                  entityData["TransformComponent"]["position"][2]),
                        entityData["TransformComponent"]["rotation"],
                        glm::vec2(entityData["TransformComponent"]["scale"][0],
                                  entityData["TransformComponent"]["scale"][1])
                    );

                    registry.emplace<RenderComponent>(
                        entity,
                        textureName,
                        RenderComponent::defaultTexCoords(),
                        Color::white()
                    );
                    break;
                }
            }
        }
    }
};
