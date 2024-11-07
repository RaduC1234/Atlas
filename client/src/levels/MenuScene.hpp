#pragma once

#include "core/Core.hpp"
#include "scene/Scene.hpp"
#include "scene/Entity.hpp"

class MenuScene : public Scene {
public:
    void onCreate() override {
        this->camera = Camera({0,0}, 2.0f);
        this->font = CreateRef<Font>("assets/fonts/Roboto-Light.ttf");
        this->texture = CreateRef<Texture>("assets/textures/default.png");
    }

    void onStart() override {

    }

    void onUpdate(float deltaTime) override {
        if (Keyboard::isKeyPressed(Keyboard::D))
            camera.position({deltaTime * 200.0f, 0});

        if (Keyboard::isKeyPressed(Keyboard::A))
            camera.position({-deltaTime * 200.0f, 0});

        if (Keyboard::isKeyPressed(Keyboard::W))
            camera.position({0, deltaTime * 200.0f});

        if (Keyboard::isKeyPressed(Keyboard::S))
            camera.position({0, -deltaTime * 200.0f});

        RenderManager::drawQuad({-50.0f, 25.0f, 0.0f}, {100.0f, 100.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, Sprite(texture));
        RenderManager::drawText({-65.0f,0,0}, "Atlas OpenGL", font, 2.0f, Color(1.0f,1.0f,1.0f,1.0f));
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
