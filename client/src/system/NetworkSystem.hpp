#pragma once

#include <Atlas.hpp>
#include <cpr/cpr.h>

struct SyncData {
    entt::registry* registry;  // ✅ Allows null and dynamic assignment
    uint64_t playerId;

    SyncData(entt::registry* reg, uint64_t pid)
        : registry(reg), playerId(pid) {}
};


class NetworkSystem {
public:
    NetworkSystem();
    ~NetworkSystem();

    void update(float deltaTime, entt::registry &registry, uint64_t playerId);

private:
    void syncEntities(entt::registry &registry, uint64_t playerId);

    void predictEntities(entt::registry &registry, float deltaTime);

    void overwriteRegistry(const nlohmann::json &jsonResponse, entt::registry &registry, uint64_t playerId);
    void networkThread();

    std::atomic<bool> isSyncing{false};
    std::queue<SyncData> syncQueue;
    std::mutex queueMutex;
    std::thread syncThread;
    std::atomic<bool> stopThread{false};
};
