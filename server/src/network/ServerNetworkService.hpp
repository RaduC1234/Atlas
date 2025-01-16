#pragma once

#include <Atlas.hpp>
#include <crow.h>

#include "Lobby.hpp"
#include "data/DatabaseManager.hpp"
#include "matchmaking/MatchmakingManager.hpp"

enum class GameMode {
    HEX_DUEL,
    HEX_ARENA
};


struct QueuedPlayer {
    uint64_t playerId;
    Player player;
    GameMode mode;
    std::chrono::system_clock::time_point queueTime;
};

class ServerNetworkService {
public:
    ServerNetworkService();
    ~ServerNetworkService();
    void start(const uint32_t port);
    void stop();

private:
    std::atomic_bool running;
    std::mutex handlerMutex;
    std::mutex queueMutex;
    std::thread matchmakingThread;
    std::vector<Lobby> lobbies;
    std::vector<QueuedPlayer> matchmakingQueue;
    std::unordered_map<uint64_t, Player> players;

    crow::response handleSyncRequest(uint64_t playerId, nlohmann::json input);

    static uint64_t generateUniqueId();
    void matchmakingLoop();
    void tryMatchDuel();
    void tryMatchArena();
    static bool validateMatchRequirements(const std::vector<QueuedPlayer>& players, GameMode mode);
    std::vector<QueuedPlayer> getPlayersInQueue(GameMode mode);
    void removeFromQueue(const std::vector<uint64_t>& playerIds);
    void createMatch(const std::vector<QueuedPlayer>& matchedPlayers, GameMode mode);
    crow::response handleJoinMatch(const crow::request& req);
};