#pragma once

#include "core/Core.hpp"
#include "core/Window.hpp"
#include "scene/Scene.hpp"

class ClientApplication {
public:
    void run();
    void changeScene(const std::string& name);
private:
    Scope<Window> window;
    Scope<Scene> currentScene;
};


