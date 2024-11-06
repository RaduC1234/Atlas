#pragma once

#include "core/Core.hpp"
#include "core/Window.hpp"
#include "scene/Scene.hpp"

class ClientApplication {
public:
    void run();
    void changeScene(Scope<Scene> scene);
private:
    Scope<Window> window;
    Scope<Scene> currentScene;

    std::atomic<bool> isRunning = true;
};


