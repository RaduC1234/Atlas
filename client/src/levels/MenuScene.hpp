#pragma once

#include <imgui.h>

#include "component/Components.hpp"
#include "core/Core.hpp"
#include "renderer/Font.hpp"
#include "resource/ResourceManager.hpp"
#include "scene/Entity.hpp"
#include "scene/Scene.hpp"
#include "system/PawnSystem.hpp"
#include "system/RenderSystem.hpp"

class MenuScene : public Scene {
public:
    void onCreate() override {
        this->camera = Camera({0, 0}, 2.0f);
        loadResources();
        createPlayer();
        createFountain();
        createUIElements();

        // Initialize systems
        renderSystem = CreateRef<RenderSystem>();
        pawnSystem = CreateRef<PawnSystem>();
    }

    void onStart() override {}

    void onUpdate(float deltaTime) override {ImGui::Begin("Debug Window");
        ImGuiIO& io = ImGui::GetIO();
        ImGui::Text("FPS: %.1f", io.Framerate);
        ImGui::Text("Screen: (%1.f, %1.f)", Mouse::getX(), Mouse::getY());


        // Update systems
        pawnSystem->update(deltaTime, registry, camera);
        renderSystem->update(deltaTime, registry);
    }

    void onRender(int screenWidth, int screenHeight) override {
        RenderManager::flush(screenWidth, screenHeight, camera);

        auto coords = this->camera.screenToWorld({Mouse::getX(), Mouse::getY()});
        ImGui::Text("World: (%1.f, %1.f)", coords.x, coords.y);
        ImGui::End();
    }

    void onDestroy() override {}

protected:
    void loadResources() {
        ResourceManager::load<Font>("font", "assets/fonts/Roboto-Light.ttf");

        // Character Sprites
        ResourceManager::load<Texture>("characterW", "assets/textures/blueMageSprite/back1.png");
        ResourceManager::load<Texture>("characterW_Alt", "assets/textures/blueMageSprite/back2.png");
        ResourceManager::load<Texture>("characterA", "assets/textures/blueMageSprite/left1.png");
        ResourceManager::load<Texture>("characterA_Alt", "assets/textures/blueMageSprite/left2.png");
        ResourceManager::load<Texture>("characterS", "assets/textures/blueMageSprite/front1.png");
        ResourceManager::load<Texture>("characterS_Alt", "assets/textures/blueMageSprite/front2.png");
        ResourceManager::load<Texture>("characterD", "assets/textures/blueMageSprite/right1.png");
        ResourceManager::load<Texture>("characterD_Alt", "assets/textures/blueMageSprite/right2.png");

        // Fountain Sprites
        ResourceManager::load<Texture>("fountainCorner1", "assets/textures/fountain/fountain1.png");
        ResourceManager::load<Texture>("fountainCorner2", "assets/textures/fountain/fountain2.png");
        ResourceManager::load<Texture>("fountainCorner3", "assets/textures/fountain/fountain3.png");
        ResourceManager::load<Texture>("fountainCorner4", "assets/textures/fountain/fountain4.png");
    }

    void createPlayer() {
        playerEntity = registry.create();

        // Add transform component
        auto& transform = registry.emplace<TransformComponent>(
            playerEntity,
            glm::vec3(0.0f, 0.0f, 0.0f),
            0.0f,
            glm::vec2(100.0f, 100.0f)
        );

        // Add render component
        auto& render = registry.emplace<RenderComponent>(
            playerEntity,
            "characterS",
            TextureCoords(),
            glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)
        );

        // Add pawn component
        auto& pawn = registry.emplace<PawnComponent>(playerEntity);
        pawn.serverId = 1; // Assign a unique server ID
    }

    void createFountainQuad(const glm::vec3& position, const std::string& textureKey) {
        auto entity = registry.create();

        registry.emplace<TransformComponent>(
            entity,
            position,
            0.0f,
            glm::vec2(100.0f, 100.0f)
        );

        registry.emplace<RenderComponent>(
            entity,
            textureKey,
            TextureCoords(),
            glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)
        );
    }

    void createFountain() {
        createFountainQuad({0.0f, 100.0f, 2.0f}, "fountainCorner1");
        createFountainQuad({100.0f, 100.0f, 2.0f}, "fountainCorner2");
        createFountainQuad({0.0f, 0.0f, 2.0f}, "fountainCorner3");
        createFountainQuad({100.0f, 0.0f, 2.0f}, "fountainCorner4");
    }

    void createUIElements() {
        auto textEntity = registry.create();

        registry.emplace<TransformComponent>(
            textEntity,
            glm::vec3(-65.0f, 0.0f, 1.0f),
            0.0f,
            glm::vec2(2.0f, 2.0f)
        );

        registry.emplace<RenderComponent>(
            textEntity,
            "Atlas OpenGL",
            "font"
        );
    }

private:
    Camera camera;
    Registry registry;
    Actor playerEntity;

    Ref<RenderSystem> renderSystem;
    Ref<PawnSystem> pawnSystem;
};