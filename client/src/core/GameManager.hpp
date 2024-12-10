#pragma once
#include <Atlas.hpp>

class AtlasClient;

struct SelfUser {
    std::string username;
    std::string mmr;
};

class GameManager {
public:
    template<typename T>
    static void changeScene();

    static void setSelfUser(const SelfUser& self);

    static SelfUser getSelfUser();

    static void initAndSet(AtlasClient* client);

private:
    inline static Scope<SelfUser> selfUser{nullptr};
    inline static AtlasClient* atlasClient{nullptr};
    inline static std::type_index currentSceneType = typeid(void);
};
