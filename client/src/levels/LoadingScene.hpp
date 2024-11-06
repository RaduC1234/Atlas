#pragma once

#include "scene/Scene.hpp"
#include "renderer/Camera.hpp"
#include "renderer/RenderManager.hpp"

class LoadingScene : public Scene {
public:

    void onCreate() override {
        this->camera = Camera({0,0}, 2.0f);
        this->font = CreateRef<Font>("assets/fonts/PixelifySans-Medium.ttf");
    }

    void onStart() override {

    }

    void onUpdate(float deltaTime) override {
        RenderManager::drawText({0,0,0}, "Connecting to Server...", font, 1.0f, Color(1.0f,1.0f,1.0f,1.0f));
    }

    void onRender(int screenWidth, int screenHeight) override {
        RenderManager::flush(screenWidth, screenHeight, camera);
    }

    void onDestroy() override {

    }

protected:

    Ref<Font> font;
    Camera camera;
};


