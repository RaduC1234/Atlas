#pragma once

#include <Atlas.hpp>

class Lobby {
public:
    Lobby() = default;

    Lobby(const Lobby &) = delete; // to prevent Registry copy - invalid
    Lobby &operator=(const Lobby &) = delete;

    Lobby(Lobby &&other) noexcept;

    Lobby &operator=(Lobby &&other) noexcept;

    bool containsPlayer(uint64_t playerId);

    void serializeRegistry(nlohmann::json &outJson);

    uint64_t nextId();

    Registry & getRegistry() {
        return registry;
    }

    std::mutex & getRegistryMutex() {
        return registryMutex;
    }

    std::vector<uint64_t> & getPlayerList() {
        return players;
    }

private:
    Registry registry;
    std::mutex registryMutex;
    std::vector<uint64_t> players;
    uint64_t entId = 0;
};
