#pragma once

#include <imgui.h>

#include "core/Core.hpp"
#include "renderer/Color.hpp"
#include "renderer/Font.hpp"
#include "resource/ResourceManager.hpp"
#include "scene/Entity.hpp"
#include "scene/Scene.hpp"

class MenuScene : public Scene {
public:
    void onCreate() override {
        //ImGui::Begin("Debug Window");

        this->camera = Camera({0, 0}, 2.0f);

        ResourceManager::load<Font>("font", "assets/fonts/Roboto-Light.ttf");

        ResourceManager::load<Texture>("characterW", "assets/textures/blueMageSprite/B.png");
        ResourceManager::load<Texture>("characterA", "assets/textures/blueMageSprite/L.png");
        ResourceManager::load<Texture>("characterS", "assets/textures/blueMageSprite/F.png");
        ResourceManager::load<Texture>("characterD", "assets/textures/blueMageSprite/R.png");

        this->characterWD = CreateRef<Texture>("assets/textures/blueMageSprite/BR.png");
        this->characterWA = CreateRef<Texture>("assets/textures/blueMageSprite/BL.png");
        this->characterSD = CreateRef<Texture>("assets/textures/blueMageSprite/FR.png");
        this->characterSA = CreateRef<Texture>("assets/textures/blueMageSprite/FL.png");

        this->currentSprite = CreateRef<Texture>("assets/textures/blueMageSprite/F.png");

        this->fountainCorner1 = CreateRef<Texture>("assets/textures/fountain/fountain1.png");
        this->fountainCorner2 = CreateRef<Texture>("assets/textures/fountain/fountain2.png");
        this->fountainCorner3 = CreateRef<Texture>("assets/textures/fountain/fountain3.png");
        this->fountainCorner4 = CreateRef<Texture>("assets/textures/fountain/fountain4.png");
    }

    void onStart() override {
    }

    void onUpdate(float deltaTime) override {
        //ImGuiIO &io = ImGui::GetIO();
        //ImGui::Text("FPS: %.1f", io.Framerate);
        //ImGui::Text("Screen: (%1.f, %1.f)", Mouse::getX(), Mouse::getY());

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
                currentSprite = ResourceManager::get<Texture>("characterA");
            }
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
                currentSprite = ResourceManager::get<Texture>("characterD");
            }
        } else if (Keyboard::isKeyPressed(Keyboard::S)) {
            playerPosition.y -= deltaTime * 200.0f;
            currentSprite = ResourceManager::get<Texture>("characterS");
        } else if (Keyboard::isKeyPressed(Keyboard::W)) {
            playerPosition.y += deltaTime * 200.0f;
            currentSprite = ResourceManager::get<Texture>("characterW");
        }

        RenderManager::drawQuad({playerPosition.x, playerPosition.y, 0.0f}, {100.0f, 100.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, Sprite(currentSprite), true);
        RenderManager::drawText({-65.0f, 0.0f, 1.0f}, "Atlas OpenGL", ResourceManager::get<Font>("font"), 2.0f, Color(1.0f, 1.0f, 1.0f, 1.0f), true);
        Sprite sprite1 = Sprite(this->fountainCorner1);
        Sprite sprite2 = Sprite(this->fountainCorner2);
        Sprite sprite3 = Sprite(this->fountainCorner3);
        Sprite sprite4 = Sprite(this->fountainCorner4);

        float fountainSize = 100.0f;
        RenderManager::drawQuad({0.0f, 100.0f, 2.0f}, {fountainSize, fountainSize}, {1.0f, 1.0f, 1.0f, 1.0f}, Sprite(this->fountainCorner1), true);
        RenderManager::drawQuad({0.0f, 0.0f, 2.0f}, {fountainSize, fountainSize}, {1.0f, 1.0f, 1.0f, 1.0f}, Sprite(this->fountainCorner3), true);
        RenderManager::drawQuad({100.0f, 100.0f, 2.0f}, {fountainSize, fountainSize}, {1.0f, 1.0f, 1.0f, 1.0f}, Sprite(this->fountainCorner2), true);
        RenderManager::drawQuad({100.0f, 0.0f, 2.0f}, {fountainSize, fountainSize}, {1.0f, 1.0f, 1.0f, 1.0f}, Sprite(this->fountainCorner4), true);
    }


    void onRender(int screenWidth, int screenHeight) override {
        RenderManager::flush(screenWidth, screenHeight, camera);

        //ImGui::End();
    }

    void onDestroy() override {
    }

protected:
    Camera camera;
    Registry registry;

    Ref<Texture> currentSprite;

    Ref<Texture> characterWD;
    Ref<Texture> characterWA;
    Ref<Texture> characterSD;
    Ref<Texture> characterSA;

    Ref<Texture> fountainCorner1;
    Ref<Texture> fountainCorner2;
    Ref<Texture> fountainCorner3;
    Ref<Texture> fountainCorner4;
};
