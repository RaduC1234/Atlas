#pragma once

#include "core/Core.hpp"
#include "scene/Scene.hpp"
#include "scene/Entity.hpp"

class MenuScene : public Scene {
public:
    void onCreate() override {
        this->camera = Camera({0,0}, 2.0f);
        this->font = CreateRef<Font>("assets/fonts/PixelifySans-Medium.ttf");
        this->texture = CreateRef<Texture>("assets/textures/default.png");
    }

    void onStart() override {

    }

    void onUpdate(float deltaTime) override {
        RenderManager::drawQuad({0.0f, 0.0f, 0.0f}, {100.0f, 100.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, Sprite(texture));
        RenderManager::drawText({0,0,0}, "The quick brown fox jumps over", font, 2.0f, Color(0.0f,0.0f,0.0f,1.0f));
    }

    void onRender(int screenWidth, int screenHeight) override {
        RenderManager::flush(screenWidth, screenHeight, camera);
    }

    void onDestroy() override {

    }

protected:

    Camera camera;
    Registry registry;
    Ref<Font> font;
    Ref<Texture> texture;
};
