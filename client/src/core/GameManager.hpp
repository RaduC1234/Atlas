#pragma once
#include <Atlas.hpp>

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

private:
    inline static Scope<SelfUser> selfUser{nullptr};
    inline static AtlasClient* atlasClient{nullptr};
    inline static std::type_index currentSceneType = typeid(void);
};
