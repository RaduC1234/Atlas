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
        this->characterW = CreateRef<Texture>("assets/textures/blueMageSprite/B.png");
        this->characterA = CreateRef<Texture>("assets/textures/blueMageSprite/L.png");
        this->characterS = CreateRef<Texture>("assets/textures/blueMageSprite/F.png");
        this->characterD = CreateRef<Texture>("assets/textures/blueMageSprite/R.png");

        this->characterWD = CreateRef<Texture>("assets/textures/blueMageSprite/BR.png");
        this->characterWA = CreateRef<Texture>("assets/textures/blueMageSprite/BL.png");
        this->characterSD = CreateRef<Texture>("assets/textures/blueMageSprite/FR.png");
        this->characterSA = CreateRef<Texture>("assets/textures/blueMageSprite/FL.png");

        this->currentSprite = CreateRef<Texture>("assets/textures/blueMageSprite/F.png");
    }

    void onStart() override {

    }

    void onUpdate(float deltaTime) override {
        bool moved = false;

        if (Keyboard::isKeyPressed(Keyboard::A)) {
            camera.position({deltaTime * 200.0f, 0});
            if (Keyboard::isKeyPressed(Keyboard::W)) {
                camera.position({deltaTime * 75.0f, -deltaTime * 75.0f});
                currentSprite = characterWA;
            } else if (Keyboard::isKeyPressed(Keyboard::S)) {
                camera.position({deltaTime * 75.0f, deltaTime * 75.0f});
                currentSprite = characterSA;
            } else {
                currentSprite = characterA;
            }
            moved = true;
        }
        else if (Keyboard::isKeyPressed(Keyboard::D)) {
            camera.position({-deltaTime * 200.0f, 0});
            if (Keyboard::isKeyPressed(Keyboard::W)) {
                camera.position({-deltaTime * 75.0f, -deltaTime * 75.0f});
                currentSprite = characterWD;
            } else if (Keyboard::isKeyPressed(Keyboard::S)) {
                camera.position({-deltaTime * 75.0f, deltaTime * 75.0f});
                currentSprite = characterSD;
            } else {
                currentSprite = characterD;
            }
            moved = true;
        }
        else if (Keyboard::isKeyPressed(Keyboard::S)) {
            camera.position({0, deltaTime * 200.0f});
            currentSprite = characterS;
            moved = true;
        }
        else if (Keyboard::isKeyPressed(Keyboard::W)) {
            camera.position({0,-deltaTime * 200.0f});
            currentSprite = characterW;
            moved = true;
        }

        //RenderManager::drawQuad({-50.0f, 25.0f, 0.0f}, {100.0f, 100.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, Sprite(texture));
        //RenderManager::drawText({-65.0f,0,0}, "Atlas OpenGL", font, 2.0f, Color(1.0f,1.0f,1.0f,1.0f));
        RenderManager::drawQuad({0.0f,0.0f,0.0f},{100.0f,100.0f},{1.0f,1.0f,1.0f,1.0f}, Sprite(currentSprite));
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
    Ref<Texture> currentSprite;
    Ref<Texture> characterS;
    Ref<Texture> characterW;
    Ref<Texture> characterA;
    Ref<Texture> characterD;
    Ref<Texture> characterWD;
    Ref<Texture> characterWA;
    Ref<Texture> characterSD;
    Ref<Texture> characterSA;
};
