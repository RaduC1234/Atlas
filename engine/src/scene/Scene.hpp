#pragma once

#include "Entity.hpp"

#include "renderer/Renderer.hpp"
#include "resource/ResourceBundle.hpp"

class Scene {
public:

    virtual ~Scene() = default;

    virtual void onCreate(void *window) =0;

    virtual void onStart() = 0;

    virtual void onUpdate(float deltaTime) = 0;

    virtual void onRender(int screenWidth, int screenHeight) = 0;

    virtual void onDestroy() = 0;

protected:

    Scope<Renderer> renderer;
    Registry registry;
    Scope<ResourceBundle> resourceBundle;

    friend class ImGuiLayer;
    friend class SceneInitializer;
};


