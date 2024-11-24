#pragma once

#include <Atlas.hpp>

class AtlasClient {
public:
    void run();
    void changeScene(Scope<Scene> scene);
private:
    Scope<Window> window;
    Scope<Scene> currentScene;

    std::atomic_bool isRunning{true};
    Config clientConfig;
};


