#pragma once

#include <imgui.h>

#include "renderer/Camera.hpp"
#include "renderer/RenderManager.hpp"
#include "system/NetworkSystem.hpp"
#include "system/PawnSystem.hpp"
#include "system/RenderSystem.hpp"

class LevelScene : public Scene {
public:
    void onCreate() override {
        constexpr int TILE_NUMBER = 41;

        //===============================Load Resources=========================================

        // tiles
        ResourceManager::load<Texture>("default", "assets/textures/default.png");

        for (int i = 0; i <= TILE_NUMBER; i++) {
            if (i == 11 or i == 24)
                continue;

            std::string tileID = std::string("tile") + (i < 10 ? "00" : (i < 100 ? "0" : "")) + std::to_string(i);
            std::string filePath = "assets/textures/tiles/" + tileID + ".png";
            ResourceManager::load<Texture>(tileID, filePath);
        }

        // pawns
        ResourceManager::load<Texture>("back1", "assets/textures/pawns/back1.png");
        ResourceManager::load<Texture>("back2", "assets/textures/pawns/back2.png");

        ResourceManager::load<Texture>("front1", "assets/textures/pawns/front1.png");
        ResourceManager::load<Texture>("front2", "assets/textures/pawns/front1.png");

        ResourceManager::load<Texture>("left1", "assets/textures/pawns/left1.png");
        ResourceManager::load<Texture>("left2", "assets/textures/pawns/left1.png");

        ResourceManager::load<Texture>("right1", "assets/textures/pawns/right1.png");
        ResourceManager::load<Texture>("right2", "assets/textures/pawns/right1.png");

        // =========================================================================================

        Actors::createPawn(
            this->registry,
            {{0.0f, 0.0f, 0.0f}, 0.0f, {100.0f, 100.0f}},
            {"front1", Sprite::defaultTexCoords(), {1.0f, 1.0f, 1.0f, 1.0f}},
            PawnComponent()
        );
    }

    void onStart() override {
    }

    void onUpdate(float deltaTime) override {
        ImGui::Begin("Debug Window");
        ImGuiIO &io = ImGui::GetIO();
        ImGui::Text("FPS: %.1f", io.Framerate);
        ImGui::Text("Mouse Screen: (%.1f, %.1f)", Mouse::getX(), Mouse::getY());
        auto coords = this->camera.screenToWorld({Mouse::getX(), Mouse::getY()});
        ImGui::Text("Mouse World: (%.1f, %.1f)", coords.x, coords.y);

        pawnSystem.update(deltaTime, registry, 0, camera);
        renderSystem.update(deltaTime, registry);
        networkSystem.update(deltaTime, registry);

        ImGui::End();
    }


    void onRender(int screenWidth, int screenHeight) override {
        RenderManager::flush(screenWidth, screenHeight, camera);
    }

    void onDestroy() override {
    }

private:
    Camera camera{{0.0f, 0.0f}, 1.0f};
    Registry registry;

    PawnSystem pawnSystem;
    RenderSystem renderSystem;
    NetworkSystem networkSystem;
};
