#pragma once
#include <Atlas.hpp>

#include "network/ClientNetworkService.hpp"

class AtlasClient;
class Window;

struct SelfUser {
    std::string username;
    std::string mmr;
};

class GameManager {
public:
    static void changeScene(const std::string& sceneName);

    static void setSelfUser(const SelfUser& self);

    static SelfUser getSelfUser();

    static void initAndSet(AtlasClient* client);

    static Window* getWindowRef();

    static void setMatchData(uint64_t matchId, uint64_t playerId) {
        currentMatchId = matchId;
        currentPlayerId = playerId;
    }

    static uint64_t getCurrentMatchId() { return currentMatchId; }
    static uint64_t getCurrentPlayerId() { return currentPlayerId; }

private:
    inline static Scope<SelfUser> selfUser{nullptr};
    inline static AtlasClient* atlasClient{nullptr};
    inline static std::type_index currentSceneType = typeid(void);
    inline static uint64_t currentMatchId{0};
    inline static uint64_t currentPlayerId{0};
};
