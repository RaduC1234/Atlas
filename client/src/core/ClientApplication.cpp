#include "ClientApplication.hpp"

#include "core/PlatformUtils.hpp"
#include "renderer/RenderManager.hpp"
#include "levels/MenuScene.hpp"
#include "levels/LoadingScene.hpp"
#include "event/EventManager.hpp"


void ClientApplication::run() {

    Log::init();
    AT_INFO("Starting Atlas Client");

    FileSystem::setWorkingDirectory(ATLAS_WORKING_DIRECTORY);
    AT_INFO("Working directory is: {0}", ATLAS_WORKING_DIRECTORY);

    this->window = CreateScope<Window>();

    EventManager::init();
    RenderManager::init();

    AT_INFO("Client finished loading");


;    this->changeScene(CreateScope<MenuScene>());

    float beginTime = Time::getTime();
    float endTime;
    float deltaTime = -1.0f;

    while (isRunning) {

        this->window->onUpdate();
        //EventManager::pollEvents();

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
