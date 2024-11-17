#include "AtlasClient.hpp"

#include "event/EventManager.hpp"
#include "levels/LoadingScene.hpp"
#include "levels/MenuScene.hpp"
#include "renderer/RenderManager.hpp"
#include "resource/ResourceManager.hpp"
#include "utils/PlatformUtils.hpp"


void AtlasClient::run() {

    Log::init();
    AT_INFO("Starting Atlas Client");

    FileSystem::setWorkingDirectory(ATLAS_WORKING_DIRECTORY);
    AT_INFO("Working directory is: {0}", ATLAS_WORKING_DIRECTORY);

    AT_TRACE("Attempting to read config file...");
    TRY_CATCH(this->clientConfig = Config::build("client.config"), AT_FATAL("Error reading config file. Exiting..."););
    AT_INFO("Config file read successfully");

    this->window = CreateScope<Window>(this->clientConfig["window_title"].toString());

    EventManager::init();
    RenderManager::init();
    ImGuiLayer::init();

    AT_INFO("Client finished loading");

    this->changeScene(CreateScope<MenuScene>());

    float beginTime = Time::now().toSeconds();
    float endTime;
    float deltaTime = -1.0f;

    while (isRunning) {


        //EventManager::pollEvents();

        if(deltaTime >= 0 && this->currentScene != nullptr) {
            ImGuiLayer::onUpdate(deltaTime);
            this->currentScene->onUpdate(deltaTime);
            this->currentScene->onRender(this->window->getWidth(), this->window->getHeight());
            ImGuiLayer::onImGuiRender();
        }

        this->window->onUpdate();

        endTime = Time::now().toSeconds();
        deltaTime = endTime - beginTime;
        beginTime = endTime;
    }

    ResourceManager::clearAll();
    ImGuiLayer::shutdown();
    RenderManager::shutdown();
}

void AtlasClient::changeScene(Scope<Scene> scene) {
    if (currentScene != nullptr)
        this->currentScene->onDestroy();

    this->currentScene = std::move(scene);
    currentScene->onCreate();
    currentScene->onStart();
}
