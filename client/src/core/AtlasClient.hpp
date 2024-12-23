#pragma once

#include <Atlas.hpp>
#include "scene/Scene.hpp"
#include "window/Window.hpp"

class AtlasClient {
public:
    void run();
    void changeScene(const std::string& sceneName);
    void shutdown();

    Window* getWindow();
private:
    Scope<Window> window;
    Scope<Scene> currentScene;

    std::atomic_bool isRunning{true};
    Config clientConfig;

    std::unordered_map<std::string, std::function<Scope<Scene>()>> sceneFactories;
    std::optional<std::string> requestedScene;

    void internalChangeScene(const std::string& sceneName);
};


