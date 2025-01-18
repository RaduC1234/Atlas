#pragma once

#include <imgui.h>
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <entt/entt.hpp>
#include "renderer/Color.hpp"
#include "window/Keyboard.hpp"
#include "window/Mouse.hpp"

/**
 * @author Radu
 */
class NetworkSystem {
public:
    NetworkSystem() : resolver(ioContext), websocketStream(ioContext), connected(false) {
        try {
            auto const results = resolver.resolve("localhost", "8080");

            boost::asio::connect(websocketStream.next_layer(), results.begin(), results.end());

            this->websocketStream.handshake("localhost", "/sync_entities_ws");
            this->connected = true;
            AT_INFO("WebSocket connection established");;

            readerThread = std::thread(&NetworkSystem::readServerUpdates, this);
        } catch (const std::exception &e) {
            AT_ERROR("WebSocket initialization error: ", e.what());
            this->connected = false;
        }
    }

    ~NetworkSystem() {
        if (connected) {
            try {
                websocketStream.close(boost::beast::websocket::close_code::normal);
                AT_INFO("WebSocket connection closed.");
            } catch (const std::exception &e) {
                AT_ERROR("Error closing WebSocket: ", e.what());
            }
        }

        if (readerThread.joinable()) {
            readerThread.join();
        }
    }

    // local deltaTime - only for predictions
    void update(float deltaTime, entt::registry &registry, const Camera &camera) {
        if (connected) {
            sendInput(camera);
            processUpdates(registry);
        } else {
            std::cerr << "WebSocket is not connected. Update skipped." << std::endl;
        }
    }

    void setPlayerId(uint64_t playerId) {
        this->playerId = playerId;
    }

private:
    void sendInput(const Camera &camera) {
        nlohmann::json input;

        auto playerScreenCoords = camera.worldToScreen(playerPos);
        auto cursorPos = Mouse::getPosition();

        auto delta = glm::vec2(cursorPos.first - playerScreenCoords.x,
                               cursorPos.second - playerScreenCoords.y);

        float angleRadians = glm::atan(delta.y, delta.x);

        // Normalize
        if (angleRadians < 0) {
            angleRadians += glm::two_pi<float>();
        }

        ImGui::Text("Pos: %.2f, %.2f, Angle: %.2f", playerPos.x, playerPos.y, glm::degrees(angleRadians));

        input["playerId"] = this->playerId;
        input["input"] = {
            {"moveBackward", Keyboard::isKeyPressed(Keyboard::S)},
            {"moveForward", Keyboard::isKeyPressed(Keyboard::W)},
            {"moveRight", Keyboard::isKeyPressed(Keyboard::D)},
            {"moveLeft", Keyboard::isKeyPressed(Keyboard::A)},
            {"aimRotation", angleRadians},
            {"isShooting",Mouse::isButtonPressed(Mouse::ButtonLeft) }
        };

        try {
            websocketStream.write(boost::asio::buffer(input.dump()));
        } catch (const std::exception &e) {
            AT_ERROR("Error sending input to server: {}", e.what());
        }
    }


    void readServerUpdates() {
        while (connected) {
            try {
                boost::beast::flat_buffer buffer;
                websocketStream.read(buffer);

                auto response = nlohmann::json::parse(boost::beast::buffers_to_string(buffer.data())); {
                    std::lock_guard<std::mutex> lock(updateMutex);
                    updateQueue.push(response);
                }
                updateCondition.notify_one();
            } catch (const std::exception &e) {
                AT_ERROR("WebSocket error during read: ", e.what());
            }
        }
    }

    void processUpdates(entt::registry &registry) {
        std::queue<nlohmann::json> local_queue; {
            std::lock_guard<std::mutex> lock(updateMutex);
            std::swap(local_queue, updateQueue);
        }

        while (!local_queue.empty()) {
            auto update = local_queue.front();
            local_queue.pop();

            overwriteRegistry(update, registry);
        }
    }

    void overwriteRegistry(const nlohmann::json &jsonResponse, entt::registry &registry) {
        std::unordered_map<uint64_t, entt::entity> existingEntities;

        auto view = registry.view<NetworkComponent>();
        for (auto entity: view) {
            auto &netComp = view.get<NetworkComponent>(entity);
            existingEntities[netComp.networkId] = entity;
        }

        for (const auto &entityData: jsonResponse["entities"]) {
            uint64_t networkId = entityData["networkId"].get<uint64_t>();
            entt::entity entity;
            bool isThePlayer = false;

            if (existingEntities.contains(networkId)) {
                entity = existingEntities[networkId];
            } else {
                entity = registry.create();
                registry.emplace<NetworkComponent>(entity, networkId);
            }

            // update
            auto tileCode = entityData["tile-code"].get<uint32_t>();
            std::string textureName;

            if (tileCode >= EntityCode::TILE_CODE && tileCode < EntityCode::TILE_CODE + EntityCode::NEXT) {
                textureName = std::format("tile_{:04}", tileCode % TILE_CODE);
            }

            if (entityData.contains("PawnComponent")) {
                auto id = entityData["PawnComponent"]["playerId"];
                auto &pawnComp = registry.get_or_emplace<PawnComponent>(entity, id, false, false, false, false, 0.0f);
                pawnComp.moveForward = entityData["PawnComponent"]["moveForward"];
                pawnComp.moveBackwards = entityData["PawnComponent"]["moveBackwards"];
                pawnComp.moveLeft = entityData["PawnComponent"]["moveLeft"];
                pawnComp.moveRight = entityData["PawnComponent"]["moveRight"];
                pawnComp.aimRotation = entityData["PawnComponent"]["aimRotation"];

                if (!pawnComp.moveForward && !pawnComp.moveBackwards && !pawnComp.moveLeft && !pawnComp.moveRight) {
                    textureName = "front1";
                }

                if (id == this->playerId) {
                    isThePlayer = true;
                }
            }

            if (entityData.contains("TransformComponent")) {
                auto pos = glm::vec3(
                    entityData["TransformComponent"]["position"][0],
                    entityData["TransformComponent"]["position"][1],
                    entityData["TransformComponent"]["position"][2]
                );

                if (isThePlayer) {
                    this->playerPos = pos;
                }

                registry.emplace_or_replace<TransformComponent>(
                    entity,
                    pos,
                    entityData["TransformComponent"]["rotation"],
                    glm::vec2(entityData["TransformComponent"]["scale"][0],
                              entityData["TransformComponent"]["scale"][1])
                );

                registry.emplace_or_replace<RenderComponent>(
                    entity,
                    textureName,
                    RenderComponent::defaultTexCoords(),
                    Color::white()
                );
            }
        }
    }

    uint64_t playerId{0};
    glm::vec3 playerPos{0.0f, 0.0f, 0.0f};

    boost::asio::io_context ioContext;
    boost::asio::ip::tcp::resolver resolver;
    boost::beast::websocket::stream<boost::asio::ip::tcp::socket> websocketStream;
    bool connected;

    std::queue<nlohmann::json> updateQueue; // store incoming updates
    std::mutex updateMutex;
    std::condition_variable updateCondition;
    std::thread readerThread;
};
