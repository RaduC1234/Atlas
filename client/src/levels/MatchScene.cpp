#include "MatchScene.hpp"

#include <imgui.h>
#include <cpr/cpr.h>

#include "renderer/Color.hpp"
#include "renderer/Font.hpp"
#include "renderer/Texture.hpp"
#include "resource/ResourceManager.hpp"
#include "window/Mouse.hpp"

void MatchScene::onCreate() {
    //===============================Load Resources=========================================

    ResourceManager::load<Font>("pixelify", "assets/fonts/PixelifySans-Medium.ttf");
    ResourceManager::load<Font>("roboto", "assets/fonts/Roboto-Light.ttf");
    ResourceManager::load<Font>("minecraft", "assets/fonts/Minecraft.ttf");
    ResourceManager::load<Font>("inter", "assets/fonts/InterVariable.ttf");
    ResourceManager::load<Font>("thaleah", "assets/fonts/ThaleahFat.ttf");

    ResourceManager::loadFromDirectory<Texture>("assets/textures", nullptr, ".png", ".jpg");

    // =========================================================================================
}

void MatchScene::onStart() {
    auto response = cpr::Get(cpr::Url{"http://127.0.0.1:8080/join_match"});
    if (response.status_code == 200) {
        this->playerId = std::stoull(response.text);
    }
}

void MatchScene::onUpdate(float deltaTime) {
    ImGui::Begin("Debug Window");
    ImGuiIO &io = ImGui::GetIO();
    ImGui::Text("FPS: %.1f", io.Framerate);
    ImGui::Text("Mouse Screen: (%.1f, %.1f)", Mouse::getX(), Mouse::getY());
    auto coords = this->camera.screenToWorld({Mouse::getX(), Mouse::getY()});
    ImGui::Text("Mouse World: (%.1f, %.1f)", coords.x, coords.y);

    static bool wireFrame = false;
    ImGui::Checkbox("Wireframe", &wireFrame);

    if (wireFrame) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    } else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
    ImGui::SameLine();
    static bool syncWithServer = true;
    ImGui::Checkbox("Sync", &syncWithServer);

    static float zoom = 1.0f;
    if (ImGui::SliderFloat("Zoom", &zoom, 0.1f, 2.0f, "%.1f")) {
        // Ensure stepping of 0.1
        zoom = round(zoom * 10.0f) / 10.0f;
        camera.setZoom(zoom);
    }

    if (syncWithServer)
        networkSystem.update(deltaTime, registry, this->playerId);

    pawnSystem.update(deltaTime, registry, this->playerId, this->camera);
}

void MatchScene::onRender(int screenWidth, int screenHeight) {
    renderSystem.update(registry);
    RenderManager::flush(screenWidth, screenHeight, camera);

    ImGui::End();
}

void MatchScene::onDestroy() {
}
