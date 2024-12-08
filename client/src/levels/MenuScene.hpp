#pragma once

#include <imgui.h>
#include <Atlas.hpp>
#include <sqlite_orm/sqlite_orm.h>

#include "renderer/Color.hpp"
#include "renderer/Font.hpp"
#include "system/PawnSystem.hpp"
#include "system/RenderSystem.hpp"
#include "system/UISystem.hpp"
#include "UI/Button.hpp"


class MenuScene : public Scene {
public:
    void onCreate() override {
        constexpr int TILE_NUMBER = 42;

        //===============================Load Resources=========================================

        ResourceManager::load<Font>("pixelify", "assets/fonts/PixelifySans-Medium.ttf");
        ResourceManager::load<Font>("roboto", "assets/fonts/Roboto-Light.ttf");
        ResourceManager::load<Font>("minecraft", "assets/fonts/Minecraft.ttf");
        ResourceManager::load<Font>("inter", "assets/fonts/InterVariable.ttf");

        // load all textures from textures directory and subdirectories
        ResourceManager::loadFromDirectory<Texture>("assets/textures", nullptr, ".png", ".jpg");

        // =========================================================================================

        // background
       this->background = Actors::createStaticProp(this->registry, // here
                                              {glm::vec3(0.0f, 0.0f, 0.0f), 0.0f, glm::vec2(1000.0f, 500.0f)},
                                              {"background01", RenderComponent::defaultTexCoords(), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), true}
        );

        std::string text = "";
        Actors::createTextbox(this->registry,
                              {glm::vec3(0.0f, 0.0f, 0.0f), 0.0f, glm::vec2(1000.0f, 100.0f)},
                              {"", RenderComponent::defaultTexCoords(), glm::vec4(1.0f, 1.0f, 1.0, 1.0f), true},
                              {text, "pixelify", glm::vec4(0.0f, 0.0f, 1.0, 1.0f)}
        );

        Actors::createTextbox(this->registry,
                              {glm::vec3(0.0f, -200.0f, 0.0f), 0.0f, glm::vec2(1000.0f, 100.0f)},
                              {"", RenderComponent::defaultTexCoords(), glm::vec4(1.0f, 1.0f, 1.0, 1.0f), true},
                              {text, "pixelify", glm::vec4(1.0f, 0.0f, 1.0, 1.0f)}
        );

        /*// Button style
        UIStyle buttonStyle;
        buttonStyle.setColors(
            {1.0f, 1.0f, 1.0f, 1.0f},  // Normal
            {1.0f, 1.0f, 1.0f, 0.9f},  // Hover
            {1.0f, 1.0f, 1.0f, 0.8f},  // Pressed
            {0.7f, 0.7f, 0.7f, 0.5f}   // Disabled
        )
        .setTextColor({0.1f, 0.1f, 0.1f, 1.0f})  // Dark text
        .setFont("minecraft")
        .setFontSize(3.0f)
        .setPadding(10.0f);

        // Create button sprites
        ButtonSprite defaultSprite("button");

        float centerX = 0.0f;
        float startY = 100.0f;
        float buttonSpacing = 300.0f;
        glm::vec2 buttonSize{800.0f, 250.0f};

        // Create buttons
        auto playButton = Button::create(
            registry,
            {centerX, startY, 0},
            buttonSize,
            "Play",
            [this]() {},
            buttonStyle,
            defaultSprite
        );

        auto optionsButton = Button::create(
            registry,
            {centerX, startY - buttonSpacing, 0},
            buttonSize,
            "Options",
            [this]() {},
            buttonStyle,
            defaultSprite
        );

        auto exitButton = Button::create(
            registry,
            {centerX, startY - buttonSpacing * 2, 0},
            buttonSize,
            "Exit",
            [this]() {},
            buttonStyle,
            defaultSprite
        );*/
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
        ImGui::End();

        //Button::update(registry, camera);
        uiSystem.update(deltaTime, registry, camera);
        renderSystem.update(deltaTime, registry);
    }


    void onRender(int screenWidth, int screenHeight) override {
        // Convert the screen dimensions to world coordinates using the camera
        glm::vec2 topLeft = camera.screenToWorld({0.0f, 0.0f}); // Top-left corner
        glm::vec2 bottomRight = camera.screenToWorld({(float)screenWidth, (float)screenHeight}); // Bottom-right corner

        // Calculate the width and height in world coordinates
        float worldWidth = bottomRight.x - topLeft.x;
        float worldHeight = topLeft.y - bottomRight.y; // Negative because of coordinate system

        // Access and update the background's TransformComponent
        auto &backgroundTransform = registry.get<TransformComponent>(this->background);
        backgroundTransform.scale = glm::vec2(worldWidth, worldHeight); // Set the scale to match the world size

        RenderManager::flush(screenWidth, screenHeight, camera);
    }

    void onDestroy() override {
        ResourceManager::clearAll();
    }

private:
    Camera camera{{0.0f, 0.0f}, 0.5f};
    Registry registry;

    RenderSystem renderSystem;
    PawnSystem pawnSystem;
    UISystem uiSystem;

    Actor background;
};
