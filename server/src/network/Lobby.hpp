#pragma once

#include <Atlas.hpp>

class Lobby {
public:
    Lobby();

    Lobby(const Lobby &) = delete; // to prevent Registry copy - invalid
    Lobby &operator=(const Lobby &) = delete;

    Lobby(Lobby &&other) noexcept;

    Lobby &operator=(Lobby &&other) noexcept;

    bool containsPlayer(uint64_t playerId);

    void serializeRegistry(nlohmann::json &outJson);

    uint64_t nextId();

    entt::registry & getRegistry() {
        return registry;
    }

    std::mutex & getRegistryMutex() {
        return registryMutex;
    }

    const std::vector<uint64_t>& getPlayerList() const {
        return players;
    }

    // Non-const access for modifications
    std::vector<uint64_t>& getPlayerList() {
        return players;
    }

    // Alternative: Add a dedicated method for adding players
    void addPlayer(uint64_t playerId) {
        players.push_back(playerId);
    }

    uint64_t getId() const {
        return entId;  // Using entId as the lobby ID
    }
private:
    entt::registry registry;
    std::mutex registryMutex;
    std::vector<uint64_t> players;
    uint64_t entId = 0;

    // I was forced to write like this. I know it s any pattern and bad.
    const std::unordered_map<uint32_t, uint32_t> tileConversion =
    {
        {0, 48}, // path - path
        {1, 42}, // grass - path with stones
        {2, 56}, // bush - mine cart
        {3, 63}, // destructible wall - create
        {4, 0}, // indisputable wall
        {5, 48},
        {6, 48},
        {7, 48}
    };
};
