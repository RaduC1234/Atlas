#include "MatchScene.hpp"

#include <imgui.h>
#include <cpr/cpr.h>

#include "core/GameManager.hpp"
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

    constexpr float mapSize = 50.0f * 100.0f; // 5000 units total
    cameraController.setBounds({mapSize, mapSize}); // This is the total size, camera system will center it
    cameraController.setSmoothness(0.05f);   // Enable smooth following
    cameraController.setZoom(0.5f);
}

void MatchScene::onStart() {
    this->playerId = ClientNetworkService::getCurrentPlayerId();
    nlohmann::json data;
    data["playerId"] = this->playerId;

    auto response = cpr::Get(
        cpr::Url{"http://127.0.0.1:8080/join_match"},
        cpr::Header{{"Content-Type", "application/Json"}},
         cpr::Body{data.dump()}
         );

    if (response.status_code == 200) {
        this->networkSystem.setPlayerId(this->playerId);
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
    float minZoom = camera.getMinimumZoomLevel();
    if (ImGui::SliderFloat("Zoom", &zoom, 0.1f, 2.0f, "%.1f")) {
        zoom = std::max(round(zoom * 10.0f) / 10.0f, minZoom);
        cameraController.setZoom(zoom);
    }

    if (syncWithServer)
        networkSystem.update(deltaTime, registry, this->camera);

    pawnSystem.update(deltaTime, registry);
    cameraController.update(registry, this->playerId, deltaTime);
}

void MatchScene::onRender(int screenWidth, int screenHeight) {
    renderSystem.update(registry);
    RenderManager::flush(screenWidth, screenHeight, camera);

    ImGui::End();
}

void MatchScene::onDestroy() {
}
