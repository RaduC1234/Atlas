//
// Created by Karina on 12/21/2024.
//
#pragma once

#include <imgui.h>
#include <Atlas.hpp>
#include "system/PawnSystem.hpp"
#include "system/RenderSystem.hpp"
#include "system/UISystem.hpp"
#include "network/ClientNetworkService.hpp"
#include "renderer/Color.hpp"

class GameModeScene : public Scene {
public:
    void onCreate() override {
        ResourceManager::load<Font>("pixelify", "assets/fonts/PixelifySans-Medium.ttf");
        ResourceManager::load<Font>("roboto", "assets/fonts/Roboto-Light.ttf");
        ResourceManager::load<Font>("minecraft", "assets/fonts/Minecraft.ttf");
        ResourceManager::load<Font>("inter", "assets/fonts/InterVariable.ttf");
        ResourceManager::load<Font>("thaleah", "assets/fonts/ThaleahFat.ttf");

        ResourceManager::loadFromDirectory<Texture>("assets/textures", nullptr, ".png", ".jpg");
    }

    void onStart() override {
        const auto &windowRef = GameManager::getWindowRef();

        int newHeight = static_cast<int>(windowRef->getHeight() * 0.75);
        int newWidth = (newHeight * 16) / 9;

        windowRef->setWindowSize(newWidth, newHeight);
        windowRef->centerWindow();
        windowRef->setWindowStyle(Window::Style::UNDECORATED);

        this->sideRect = Actors::createStaticProp(this->registry,
                                        {glm::vec3(-1100.0f, 0.0f, 1.0f), 0.0f, glm::vec2(1700.0f, 2400.0f)},
                                        {"", RenderComponent::defaultTexCoords(), Color(22,22,22,200), true});
        // Background
        this->background = Actors::createStaticProp(this->registry,
            {glm::vec3(0.0f, 0.0f, 1.0f), 0.0f, glm::vec2(1000.0f, 500.0f)},
            {"background01", RenderComponent::defaultTexCoords(), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), true});


        // Title
        this->title = Actors::createStaticProp(this->registry,
                                             {glm::vec3(-1135.0f, 470.0f, 0.0f), 0.0f, glm::vec2(15.0f, 30.0f)},
                                    {"thaleah", "Select game mode", true, Color::white()}
        );

        // 1v1 Button
        this->hexDuelButton = Actors::createButton(this->registry,
            {glm::vec3(-1100.0f, 100.0f, 0.0f), 0.0f, glm::vec2(900.0f, 200.0f)},
            {"panel-transparent-border-010", RenderComponent::defaultTexCoords(), Color::white(), true, RENDERER_NINE_SLICE},
            {"Hex Duel", "thaleah", Color::black(), false, false, [this]() {
                AT_INFO("Hex Duel selected!");
            }, nullptr, nullptr, Color::white(), Color(74, 58, 112), Color(38, 28, 61)});

        this->hexDuelDescription = Actors::createStaticProp(this->registry,
                                             {glm::vec3(-1100.0f, -100.0f, 0.0f), 0.0f, glm::vec2(10.0f, 10.0f)},
                                    {"thaleah", "1v1", true, Color::white()});

        // 1v1v1v1 Button
        this->hexArenaButton = Actors::createButton(this->registry,
            {glm::vec3(-1100.0f, -300.0f, 0.0f), 0.0f, glm::vec2(900.0f, 200.0f)},
            {"panel-transparent-border-010", RenderComponent::defaultTexCoords(), Color::white(), true, RENDERER_NINE_SLICE},
            {"Hex Arena", "thaleah", Color::black(), false, false, [this]() {
                AT_INFO("Hex Arena selected!");
            }, nullptr, nullptr, Color::white(), Color(74, 58, 112), Color(38, 28, 61)});

        this->hexArenaDescription = Actors::createStaticProp(this->registry,
                                     {glm::vec3(-1100.0f, -500.0f, 0.0f), 0.0f, glm::vec2(10.0f, 10.0f)},
                            {"thaleah", "1v1v1v1", true, Color::white()});

        // Back Button
        this->backButton = Actors::createButton(this->registry,
            {glm::vec3(-1500.0f, -800.0f, 0.0f), 0.0f, glm::vec2(500.0f, 150.0f)},
            {"panel-transparent-border-010", RenderComponent::defaultTexCoords(), Color::white(), true, RENDERER_NINE_SLICE},
            {"Back", "thaleah", Color::black(), false, false, [this]() {
                AT_INFO("Returning to Menu Scene!");
            }, nullptr, nullptr, Color::white(), Color(74, 58, 112), Color(38, 28, 61)});
    }

    void onUpdate(float deltaTime) override {
        uiSystem.update(deltaTime, registry, camera);
        renderSystem.update(deltaTime, registry);
    }

    void onRender(int screenWidth, int screenHeight) override {
        const float baseWidth = 1920.0f;
        const float baseHeight = 1080.0f;
        const float baseZoom = 0.5f;

        float aspectRatio = static_cast<float>(screenWidth) / static_cast<float>(screenHeight);
        float baseAspectRatio = baseWidth / baseHeight;

        camera.setZoom(baseZoom * (screenWidth / baseWidth));

        const glm::vec2 topLeft = camera.screenToWorld({0.0f, 0.0f});
        const glm::vec2 bottomRight = camera.screenToWorld({static_cast<float>(screenWidth), static_cast<float>(screenHeight)});

        const float worldWidth = bottomRight.x - topLeft.x;
        const float worldHeight = topLeft.y - bottomRight.y;

        if (registry.valid(background)) {
            auto &backgroundTransform = registry.get<TransformComponent>(this->background);
            backgroundTransform.scale = glm::vec2(worldWidth, worldHeight);
        }

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
    Actor sideRect;
    Actor title;
    Actor hexDuelButton;
    Actor hexDuelDescription;
    Actor hexArenaDescription;
    Actor hexArenaButton;
    Actor backButton;
};

