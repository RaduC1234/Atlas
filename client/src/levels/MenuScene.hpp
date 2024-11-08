#pragma once

#include "core/Core.hpp"
#include "scene/Scene.hpp"
#include "scene/Entity.hpp"

class MenuScene : public Scene {
public:
    void onCreate() override {
        this->camera = Camera({0, 0}, 2.0f);

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
        static glm::vec2 playerPosition = {0.0f, 0.0f};

        if (Keyboard::isKeyPressed(Keyboard::A)) {
            playerPosition.x -= deltaTime * 200.0f;
            if (Keyboard::isKeyPressed(Keyboard::W)) {
                playerPosition.x -= deltaTime * 75.0f;
                playerPosition.y += deltaTime * 75.0f;
                currentSprite = characterWA;
            } else if (Keyboard::isKeyPressed(Keyboard::S)) {
                playerPosition.x -= deltaTime * 75.0f;
                playerPosition.y -= deltaTime * 75.0f;
                currentSprite = characterSA;
            } else {
                currentSprite = characterA;
            }
            moved = true;
        } else if (Keyboard::isKeyPressed(Keyboard::D)) {
            playerPosition.x += deltaTime * 200.0f;
            if (Keyboard::isKeyPressed(Keyboard::W)) {
                playerPosition.x += deltaTime * 75.0f;
                playerPosition.y += deltaTime * 75.0f;
                currentSprite = characterWD;
            } else if (Keyboard::isKeyPressed(Keyboard::S)) {
                playerPosition.x += deltaTime * 75.0f;
                playerPosition.y -= deltaTime * 75.0f;
                currentSprite = characterSD;
            } else {
                currentSprite = characterD;
            }
            moved = true;
        } else if (Keyboard::isKeyPressed(Keyboard::S)) {
            playerPosition.y -= deltaTime * 200.0f;
            currentSprite = characterS;
            moved = true;
        } else if (Keyboard::isKeyPressed(Keyboard::W)) {
            playerPosition.y += deltaTime * 200.0f;
            currentSprite = characterW;
            moved = true;
        }

        RenderManager::drawQuad({playerPosition.x, playerPosition.y, 0.0f}, {75.0f, 75.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, Sprite(currentSprite));
        RenderManager::drawText({-65.0f, 0.0f, 1.0f}, "Atlas OpenGL", font, 2.0f, Color(1.0f, 1.0f, 1.0f, 1.0f));
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
