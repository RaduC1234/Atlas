#pragma once

#include <renderer/ImGuiLayer.h>

#include "core/Core.hpp"
#include "core/Window.hpp"
#include "scene/Scene.hpp"
#include "utils/Config.hpp"

class AtlasClient {
public:
    void run();
    void changeScene(Scope<Scene> scene);
private:
    Scope<Window> window;
    Scope<Scene> currentScene;

    std::atomic<bool> isRunning = true;
    Config clientConfig;
};


