#pragma once

#include <Atlas.hpp>
#include <crow/websocket.h>

struct PlayerInput {
    bool moveForward = false;
    bool moveBackwards = false;
    bool moveLeft = false;
    bool moveRight = false;
    float aimRotation = 0.0f;
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

    void setPlayerInput(uint64_t playerId, const PlayerInput &input);

    entt::registry &getRegistry() {
        return registry;
    }

    std::mutex &getRegistryMutex() {
        return registryMutex;
    }

    const std::vector<uint64_t> &getPlayerList() const {
        return players;
    }

    void addPlayer(uint64_t playerId) {
        players.push_back(playerId);
    }

    int getPlayersSize() {
        return this->players.size();
    }

    uint64_t getId() const {
        return entId;
    }

    bool hasStarted() const {
        return started;
    }

    void addConnection(uint64_t playerId, crow::websocket::connection *conn) {
        std::lock_guard<std::mutex> lock(playersMutex);
        playerConnections[playerId] = conn;
    }

    void removeConnection(uint64_t playerId) {
        std::lock_guard<std::mutex> lock(playersMutex);
        playerConnections.erase(playerId);
    }

private:
    const float baseSpeed = 100.0f;

    entt::registry registry;
    std::mutex registryMutex;

    std::vector<uint64_t> players;
    uint64_t entId = 0;

    bool started = false;

    std::unordered_map<uint64_t, PlayerInput> inputQueue; // Latest input for each player
    std::mutex inputMutex;

    std::unordered_map<uint64_t, crow::websocket::connection *> playerConnections;
    std::mutex playersMutex;

};

#define DIRTY_COMPONENT(clazz) \
registry.on_construct<clazz>().connect<&Lobby::markDirty>(*this); \
registry.on_update<clazz>().connect<&Lobby::markDirty>(*this); \
registry.on_destroy<clazz>().connect<&Lobby::markDirty>(*this);
