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
#include "system/AnimationSystem.hpp"
#include "UI/Button.hpp"
#include "UI/UISystem.hpp"

class MenuScene : public Scene {
public:
    void onCreate() override {
        this->camera = Camera({0, 0}, 2.0f); // Initialize the camera with zoom level 2.0f

        loadResources(); // Load assets required by the scene
        createPlayer();  // Create the player entity

        // Initialize ECS systems
        renderSystem = CreateRef<RenderSystem>();
        pawnSystem = CreateRef<PawnSystem>(200.0f, 0.1f); // Movement speed: 200, Camera smoothing: 0.1
        animationSystem = CreateRef<AnimationSystem>();

        // Validate all animations (for debugging purposes)
        animationSystem->validateAllAnimations(registry);
    }

    void onStart() override {}

    void onUpdate(float deltaTime) override {
        // Debug window using ImGui
        debugWindow();

        // Update ECS systems
        pawnSystem->update(deltaTime, registry, camera); // Handle player movement
        animationSystem->update(deltaTime, registry);    // Update animations
        renderSystem->update(deltaTime, registry);       // Handle rendering
    }

    void onRender(int screenWidth, int screenHeight) override {
        RenderManager::flush(screenWidth, screenHeight, camera); // Flush render queue
    }

    void onDestroy() override {}

protected:
    void loadResources() {
        // Load assets such as fonts and animations
        ResourceManager::load<Font>("font", "assets/fonts/Roboto-Light.ttf");
        ResourceManager::loadAnimations("assets/animations/animations.json");
    }

    void createPlayer() {
        auto playerEntity = registry.create();

        // Add TransformComponent
        registry.emplace<TransformComponent>(playerEntity,glm::vec3(0.0f, 0.0f, 0.0f),0.0f,glm::vec2(100.0f, 100.0f));

        // Add RenderComponent with default texture
        registry.emplace<RenderComponent>(playerEntity,"front1.png",TextureCoords(),glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

        // Add PawnComponent for movement handling
        registry.emplace<PawnComponent>(playerEntity);

        // Add and configure AnimationComponent
        auto& animation = registry.emplace<AnimationComponent>(playerEntity);

        // Load animations from the ResourceManager
        animation.animations = ResourceManager::loadAnimations("assets/animations/animations.json");

        // Set default animation
        animation.setAnimation("walk_down");

        // Validate animations to ensure proper setup
        animation.validateAnimations();

        // Store reference to the player entity
        this->playerEntity = playerEntity;
    }

    void debugWindow() {
        ImGui::Begin("Debug Window");

        // Display FPS
        ImGuiIO& io = ImGui::GetIO();
        ImGui::Text("FPS: %.1f", io.Framerate);

        // Display mouse position on screen and world
        ImGui::Text("Mouse Screen: (%.1f, %.1f)", Mouse::getX(), Mouse::getY());
        auto coords = this->camera.screenToWorld({Mouse::getX(), Mouse::getY()});
        ImGui::Text("Mouse World: (%.1f, %.1f)", coords.x, coords.y);

        ImGui::End();
    }

private:
    Camera camera;
    Registry registry;
    Actor playerEntity;

    // ECS Systems
    Ref<RenderSystem> renderSystem;
    Ref<PawnSystem> pawnSystem;
    Ref<AnimationSystem> animationSystem;
};
