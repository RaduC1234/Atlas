#pragma once

#include <Atlas.hpp>
#include <boost/beast/http/verb.hpp>

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

    static std::string httpRequest(const std::string& target, boost::beast::http::verb method, const std::string& body = "", const std::map<std::string, std::string>& headers = {});

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
