#include "MatchScene.hpp"

#include <imgui.h>

#include "renderer/Color.hpp"
#include "renderer/Font.hpp"
#include "renderer/Texture.hpp"
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

    //pawnSystem.update(deltaTime, registry, 0, camera);

    networkSystem.update(deltaTime, registry, this->playerId);
    pawnSystem.update(deltaTime, registry, this->playerId, this->camera);
    renderSystem.update(deltaTime, registry);

    ImGui::End();

    // Debugging: Check if entities exist
    auto view = registry.view<entt::entity>();
    size_t entityCount = std::distance(view.begin(), view.end());
    std::cout << "Rendering Entities: " << entityCount << std::endl;

    for (auto entity : view) {
        auto& transform = registry.get<TransformComponent>(entity);
        std::cout << "Entity " << static_cast<int>(entity) << " at ("
                  << transform.position.x << ", " << transform.position.y << ")\n";
    }
}

void MatchScene::onRender(int screenWidth, int screenHeight) {
    RenderManager::flush(screenWidth, screenHeight, camera);
}

void MatchScene::onDestroy() {
}
