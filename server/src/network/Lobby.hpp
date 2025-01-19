#pragma once

#include <Atlas.hpp>
#include <crow/websocket.h>

struct PlayerInput {
    bool moveForward = false;
    bool moveBackwards = false;
    bool moveLeft = false;
    bool moveRight = false;
    float aimRotation = 0.0f;
    bool isShooting = false;
};

struct PlayerSpawnPoint {
    uint64_t playerId;
    glm::vec3 position;
};

class Lobby {
public:
    Lobby();
    Lobby(const Lobby &) = delete;
    Lobby &operator=(const Lobby &) = delete;
    Lobby(Lobby &&other) noexcept;
    Lobby &operator=(Lobby &&other) noexcept;

    bool containsPlayer(uint64_t playerId);
    uint64_t nextId();
    void start();
    void update(float deltaTime);

    void markDirty(entt::registry &registry, entt::entity entity);
    void markDeleted(entt::registry &registry, entt::entity entity);
    void setPlayerInput(uint64_t playerId, const PlayerInput &input);

    entt::registry &getRegistry() { return registry; }
    std::mutex &getRegistryMutex() { return registryMutex; }
    const std::vector<uint64_t> &getPlayerList() const { return players; }

    void addPlayer(uint64_t playerId) {
        players.push_back(playerId);

        // Assign spawn point based on player index
        size_t playerIndex = players.size() - 1;
        glm::vec3 spawnPosition;

        if (players.size() <= 2) {
            // For 2 players - diagonal spawn
            switch (playerIndex) {
                case 0:
                    spawnPosition = glm::vec3(-2400, 2400, 0); // Top left
                    break;
                case 1:
                    spawnPosition = glm::vec3(2400, -2400, 0); // Bottom right
                    break;
                default:
                    spawnPosition = glm::vec3(0, 0, 0);
                    break;
            }
        } else {
            // For 3-4 players - all corners
            switch (playerIndex) {
                case 0:
                    spawnPosition = glm::vec3(-2400, -2400, 0); // Bottom left
                    break;
                case 1:
                    spawnPosition = glm::vec3(2400, -2400, 0); // Bottom right
                    break;
                case 2:
                    spawnPosition = glm::vec3(-2400, 2400, 0); // Top left
                    break;
                case 3:
                    spawnPosition = glm::vec3(2400, 2400, 0); // Top right
                    break;
                default:
                    spawnPosition = glm::vec3(0, 0, 0);
                    break;
            }
        }

        playerSpawnPoints[playerId] = {playerId, spawnPosition};
    }

    int getPlayersSize() { return this->players.size(); }
    uint64_t getId() const { return entId; }
    bool hasStarted() const { return started; }

    void addConnection(uint64_t playerId, crow::websocket::connection *conn) {
        std::lock_guard<std::mutex> lock(playersMutex);
        this->synced = false;
        playerConnections[playerId] = conn;
    }

    void removeConnection(uint64_t playerId) {
        std::lock_guard<std::mutex> lock(playersMutex);
        playerConnections.erase(playerId);
    }

    void markUnSynced() {
        this->synced = false;
    }

    // Fixed respawnPlayer function to correctly handle const references
    void respawnPlayer(uint64_t playerId) {
        auto spawnPointIt = playerSpawnPoints.find(playerId);
        if (spawnPointIt != playerSpawnPoints.end()) {
            // Use get_mut to get a non-const reference to the components
            auto view = registry.view<PawnComponent, TransformComponent>();
            for (auto entity : view) {
                auto [pawn, transform] = view.get<PawnComponent, TransformComponent>(entity);
                if (pawn.playerId == playerId) {
                    auto& mutableTransform = registry.get<TransformComponent>(entity);
                    mutableTransform.position = spawnPointIt->second.position;
                    if (auto* network = registry.try_get<NetworkComponent>(entity)) {
                        network->dirtyFlag = true;
                    }
                    break;
                }
            }
        }
    }

private:
    const float baseSpeed = 100.0f;
    const float shootCooldown = 0.5f;  // 500ms cooldown between shots
    std::unordered_map<uint64_t, float> lastShotTimes;
    std::unordered_map<uint64_t, PlayerSpawnPoint> playerSpawnPoints;
    bool isPositionInsideFireball(const glm::vec3& spawnPosition);

    bool canPlayerShoot(uint64_t playerId, float currentTime) {
        auto it = lastShotTimes.find(playerId);
        if (it == lastShotTimes.end()) {
            return true;
        }
        return (currentTime - it->second) >= shootCooldown;
    }

    void updatePlayerLastShotTime(uint64_t playerId, float currentTime) {
        lastShotTimes[playerId] = currentTime;
    }

    entt::registry registry;
    std::mutex registryMutex;
    std::vector<uint64_t> players;
    uint64_t entId = 0;

    bool started = false;

    std::unordered_map<uint64_t, PlayerInput> inputQueue;
    std::mutex inputMutex;

    std::unordered_map<uint64_t, crow::websocket::connection *> playerConnections;
    std::mutex playersMutex;

    std::vector<uint64_t> deletedEntities;

    bool synced{true};
};

#define DIRTY_COMPONENT(clazz) \
registry.on_construct<clazz>().connect<&Lobby::markDirty>(*this); \
registry.on_update<clazz>().connect<&Lobby::markDirty>(*this);