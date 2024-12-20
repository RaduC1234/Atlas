#pragma once

#include <imgui.h>

#include "renderer/Camera.hpp"
#include "renderer/RenderManager.hpp"
#include "system/NetworkSystem.hpp"
#include "system/PawnSystem.hpp"
#include "system/RenderSystem.hpp"

class MatchScene : public Scene {
public:
    void onCreate() override {
        //===============================Load Resources=========================================

        ResourceManager::load<Font>("pixelify", "assets/fonts/PixelifySans-Medium.ttf");
        ResourceManager::load<Font>("roboto", "assets/fonts/Roboto-Light.ttf");
        ResourceManager::load<Font>("minecraft", "assets/fonts/Minecraft.ttf");
        ResourceManager::load<Font>("inter", "assets/fonts/InterVariable.ttf");
        ResourceManager::load<Font>("thaleah", "assets/fonts/ThaleahFat.ttf");

        ResourceManager::loadFromDirectory<Texture>("assets/textures", nullptr, ".png", ".jpg");

        // =========================================================================================
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
