#pragma once

#include <Atlas.hpp>

#include "scene/Scene.hpp"
#include "window/Window.hpp"

class AtlasClient {
public:
    void run();
    void changeScene(Scope<Scene> scene);
    void shutdown();

    Window* getWindow();
private:
    Scope<Window> window;
    Scope<Scene> currentScene;

    std::atomic_bool isRunning{true};
    Config clientConfig;
};


