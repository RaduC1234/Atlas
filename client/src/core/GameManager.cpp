#include "GameManager.hpp"
#include "AtlasClient.hpp"

void GameManager::changeScene(const std::string &sceneName) {
    atlasClient->changeScene(sceneName);
}

void GameManager::setSelfUser(const SelfUser& self) {
    if (!selfUser) {
        selfUser = CreateScope<SelfUser>();
    }
    *selfUser = self;
}

SelfUser GameManager::getSelfUser() {
    return *selfUser;
}

void GameManager::initAndSet(AtlasClient* client) {
    atlasClient = client;
}

Window * GameManager::getWindowRef() {
    return atlasClient->getWindow();
}


// Explicit template instantiations (if needed, for specific scene types)
// template void GameManager::changeScene<MenuScene>();
// template void GameManager::changeScene<LevelScene>();
