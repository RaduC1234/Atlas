#include "NetworkSystem.hpp"

#include "renderer/Color.hpp"
#include "window/Keyboard.hpp"

NetworkSystem::NetworkSystem() {
    syncThread = std::thread(&NetworkSystem::networkThread, this);
}

NetworkSystem::~NetworkSystem() {
    stopThread = true;
    syncThread.join();
}

void NetworkSystem::update(float deltaTime, entt::registry &registry, uint64_t playerId) {
    if (!isSyncing.exchange(true)) {
        std::lock_guard<std::mutex> lock(queueMutex);
        syncQueue.push(SyncData(&registry, playerId));
    }
}

void NetworkSystem::networkThread() {
    while (!stopThread) {
        if (!syncQueue.empty()) {
            SyncData syncData(nullptr, 0);
            {
                std::lock_guard<std::mutex> lock(queueMutex);
                syncData = syncQueue.front();
                syncQueue.pop();
            }

            if (syncData.registry) {  // ✅ Prevents nullptr crash
                syncEntities(*syncData.registry, syncData.playerId);
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

void NetworkSystem::syncEntities(entt::registry &registry, uint64_t playerId) {
    nlohmann::json input;
    input["moveForward"] = Keyboard::isKeyPressed(Keyboard::W);
    input["moveBackwards"] = Keyboard::isKeyPressed(Keyboard::S);
    input["moveLeft"] = Keyboard::isKeyPressed(Keyboard::A);
    input["moveRight"] = Keyboard::isKeyPressed(Keyboard::D);

    cpr::Response response = cpr::Post(
        cpr::Url{"http://localhost:8080/sync_entities"},
        cpr::Body(nlohmann::json{{"playerId", playerId}, {"input", input}}.dump()),
        cpr::Header{{"Content-Type", "application/json"}}
    );

    if (response.status_code != 200) {
        AT_ERROR("Sync failed: ", response.status_code);
        isSyncing = false;
        return;
    }

    try {
        auto jsonResponse = nlohmann::json::parse(response.text);

        if (!jsonResponse.contains("entities") || !jsonResponse["entities"].is_array()) {
            AT_ERROR("Invalid response format from server!");
            isSyncing = false;
            return;
        }

        overwriteRegistry(jsonResponse, registry, playerId);
    } catch (const std::exception &e) {
        AT_ERROR("Error parsing sync response: {}",e.what());
    }

    isSyncing = false;
}

void NetworkSystem::overwriteRegistry(const nlohmann::json &jsonResponse, entt::registry &registry, uint64_t playerId) {
    std::unordered_map<uint64_t, entt::entity> existingEntities;

    auto view = registry.view<NetworkComponent>();
    for (auto entity: view) {
        auto &netComp = view.get<NetworkComponent>(entity);
        existingEntities[netComp.networkId] = entity;
    }

    for (const auto &entityData: jsonResponse["entities"]) {
        uint64_t networkId = entityData["networkId"].get<uint64_t>();
        entt::entity entity;

        if (existingEntities.contains(networkId)) {
            entity = existingEntities[networkId];
        } else {
            entity = registry.create();
            registry.emplace<NetworkComponent>(entity, networkId, static_cast<EntityType>(entityData["entityType"].get<int>()));
        }

        auto tileCode = entityData["tile-code"].get<uint32_t>();
        std::string textureName;

        if (tileCode >= EntityCode::TILE_CODE && tileCode < EntityCode::TILE_CODE + EntityCode::NEXT) {
            textureName = std::format("tile_{:04}", tileCode % 10000);
        }

        if (entityData.contains("PawnComponent")) {
            auto &pawnComp = registry.get_or_emplace<PawnComponent>(entity, entityData["PawnComponent"]["playerId"], false, false, false, false, 0.0f);
            pawnComp.moveForward = entityData["PawnComponent"]["moveForward"];
            pawnComp.moveBackwards = entityData["PawnComponent"]["moveBackwards"];
            pawnComp.moveLeft = entityData["PawnComponent"]["moveLeft"];
            pawnComp.moveRight = entityData["PawnComponent"]["moveRight"];

            if (!pawnComp.moveForward && !pawnComp.moveBackwards && !pawnComp.moveLeft && !pawnComp.moveRight) {
                textureName = "front1";
            }
        }

        if (entityData.contains("TransformComponent")) {
            registry.emplace_or_replace<TransformComponent>(
                entity,
                glm::vec3(entityData["TransformComponent"]["position"][0],
                          entityData["TransformComponent"]["position"][1],
                          entityData["TransformComponent"]["position"][2]),
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
