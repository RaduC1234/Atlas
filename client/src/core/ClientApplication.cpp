#include "ClientApplication.hpp"

#include "core/PlatformUtils.hpp"
#include "renderer/RenderManager.hpp"
#include "levels/MenuScene.hpp"
#include "levels/LoadingScene.hpp"


void ClientApplication::run() {

    Log::init();
    AT_INFO("Starting Atlas Client");

    this->window = CreateScope<Window>();

    RenderManager::init();

    this->changeScene(CreateScope<MenuScene>());

    float beginTime = Time::getTime();
    float endTime;
    float deltaTime = -1.0f;

    while (isRunning) {

        this->window->onUpdate();

        if(deltaTime >= 0 && this->currentScene != nullptr) {
            this->currentScene->onUpdate(deltaTime);
            this->currentScene->onRender(this->window->getWidth(), this->window->getHeight());
        }

        endTime = Time::getTime();
        deltaTime = endTime - beginTime;
        beginTime = endTime;
    }

    RenderManager::shutdown();
}

void ClientApplication::changeScene(Scope<Scene> scene) {
    if (currentScene != nullptr)
        this->currentScene->onDestroy();

    this->currentScene = std::move(scene);
    currentScene->onCreate();
    currentScene->onStart();
}
