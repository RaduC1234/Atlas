#include "GameManager.hpp"
#include "AtlasClient.hpp"

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

template<typename T>
void GameManager::changeScene() {
    if (atlasClient) {
        if (currentSceneType == typeid(T)) {
            return;
        }

        atlasClient->changeScene(CreateScope<T>());
        currentSceneType = typeid(T);
    }
}

// Explicit template instantiations (if needed, for specific scene types)
// template void GameManager::changeScene<MenuScene>();
// template void GameManager::changeScene<LevelScene>();
