#include "AtlasClient.hpp"

#include "GameManager.hpp"
#include "levels/GameModeScene.hpp"
#include "levels/LoginScene.hpp"
#include "levels/MenuScene.hpp"
#include "levels/RegisterScene.hpp"
#include "network/ClientNetworkService.hpp"
#include "renderer/ImGuiLayer.h"
#include "renderer/RenderManager.hpp"

void AtlasClient::run() {
    Log::init();
    AT_INFO("Starting Atlas Client");

    FileSystem::setWorkingDirectory(ATLAS_WORKING_DIRECTORY);
    AT_INFO("Working directory is: {0}", ATLAS_WORKING_DIRECTORY);

    AT_TRACE("Attempting to read config file...");
    TRY_CATCH(this->clientConfig = Config::build("client.config"), AT_FATAL("Error reading config file. Exiting..."););
    AT_INFO("Config file read successfully");

    this->window = CreateScope<Window>(this->clientConfig["window_title"].toString());
    this->window->setCloseCallback([this]() {
        this->shutdown(); // cleanup resources
    });

    const auto serverUrl = "http://" + this->clientConfig["server_host"].toString() + ":" + this->clientConfig["server_port"].toString();

    EventManager::init();
    RenderManager::init();
    ImGuiLayer::init();
    GameManager::initAndSet(this);
    ClientNetworkService::init(serverUrl);

    AT_INFO("Client finished loading");

    sceneFactories["LoginScene"] = []() { return CreateScope<LoginScene>(); };
    sceneFactories["MenuScene"] = []() { return CreateScope<MenuScene>(); };
    sceneFactories["RegisterScene"] = []() { return CreateScope<RegisterScene>(); };
    sceneFactories["GameModeScene"] = []() { return CreateScope<GameModeScene>(); };

    this->changeScene("LoginScene");

    float beginTime = Time::now().toSeconds();
    float endTime;
    float deltaTime = -1.0f;

    while (isRunning) {
        if (deltaTime >= 0 && this->currentScene != nullptr) {
            ImGuiLayer::onUpdate(deltaTime);
            this->currentScene->onUpdate(deltaTime);
            this->currentScene->onRender(this->window->getWidth(), this->window->getHeight());
            ImGuiLayer::onImGuiRender();
        }

        this->window->onUpdate();


        if (requestedScene) {
            this->internalChangeScene(*requestedScene);
            requestedScene.reset();
        }


        endTime = Time::now().toSeconds();
        deltaTime = endTime - beginTime;
        beginTime = endTime;
    }

    shutdown();
}

void AtlasClient::changeScene(const std::string &sceneName) {
    requestedScene = sceneName;
}

void AtlasClient::internalChangeScene(const std::string &sceneName) {
    auto it = sceneFactories.find(sceneName);
    if (it == sceneFactories.end()) {
        AT_FATAL("Scene not found: {}", sceneName);
        return;
    }

    if (currentScene != nullptr) {
        this->currentScene->onDestroy();
    }

    this->currentScene = it->second(); // Create new instance
    currentScene->onCreate();
    currentScene->onStart();
}

Window *AtlasClient::getWindow() {
    return this->window.get();
}

void AtlasClient::shutdown() {
}
