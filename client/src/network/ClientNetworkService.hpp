#pragma once

#include <Atlas.hpp>
#include <cpr/cpr.h>

#include "map/MapState.hpp"

class ClientNetworkService {
public:
    enum class GameMode {
        HEX_DUEL,
        HEX_ARENA
    };

    ClientNetworkService() = default;
    ~ClientNetworkService() = default;
    static void init(const std::string &remoteHost);

    template<typename T, typename... Args>
    static void addRequestTemplate(Args &&... args);

    template<typename T>
    static void sendRequestAsync();

    static MapState getMapData();
    static bool reg(const std::string &username, const std::string &password);
    static bool login(const std::string &username, const std::string &password);
    static bool joinMatchmaking(GameMode mode);
    static bool leaveMatchmaking();
    static bool submitMatchResult(uint64_t matchId, uint64_t winnerId);
    static uint64_t joinMatch();
    static bool checkMatchStatus();
    static uint64_t getCurrentMatchId();
    static uint64_t getCurrentPlayerId();

private:
    static inline uint64_t loginToken{0};
    static inline std::string serverUrl{};
    static inline uint64_t currentMatchId{0};
    static inline uint64_t currentPlayerId{0};
};
