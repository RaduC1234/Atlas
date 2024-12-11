#pragma once

#include <imgui.h>
#include <Atlas.hpp>

#include "renderer/Color.hpp"
#include "renderer/Font.hpp"
#include "system/PawnSystem.hpp"
#include "system/RenderSystem.hpp"
#include "system/UISystem.hpp"


class MenuScene : public Scene {
public:
    void onCreate() override {
        //===============================Load Resources=========================================

        ResourceManager::load<Font>("pixelify", "assets/fonts/PixelifySans-Medium.ttf");
        ResourceManager::load<Font>("roboto", "assets/fonts/Roboto-Light.ttf");
        ResourceManager::load<Font>("minecraft", "assets/fonts/Minecraft.ttf");
        ResourceManager::load<Font>("inter", "assets/fonts/InterVariable.ttf");

        // load all textures from textures directory and subdirectories
        ResourceManager::loadFromDirectory<Texture>("assets/textures", nullptr, ".png", ".jpg");

        // =========================================================================================

        // background
        /*this->background = Actors::createStaticProp(this->registry, // here
                                               {glm::vec3(0.0f, 0.0f, 1.0f), 0.0f, glm::vec2(1000.0f, 500.0f)},
                                               {"background01", RenderComponent::defaultTexCoords(), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), true}
         );*/

        Actors::createTextbox(this->registry,
                              {glm::vec3(0.0f, 0.0f, 0.0f), 0.0f, glm::vec2(1000.0f, 250.0f)},
                              {"panel-border-010", RenderComponent::defaultTexCoords(), Color::white(), true, RENDERER_NINE_SLICE},
                              {"", "roboto", Color::white()}
        );

        Actors::createTextbox(this->registry,
                              {glm::vec3(0.0f, -200.0f, 0.0f), 0.0f, glm::vec2(1000.0f, 100.0f)},
                              {"panel-transparent-border-010", RenderComponent::defaultTexCoords(), Color::white(), true, RENDERER_NINE_SLICE},
                              {"", "roboto", Color::black(), 5}
        );

        Actors::createButton(this->registry,
                                {glm::vec3(0.0f, -400.0f, 0.0f), 0.0f, glm::vec2(1000.0f, 200.0f)},
                                {"panel-transparent-border-010", RenderComponent::defaultTexCoords(), Color::red(), true, RENDERER_NINE_SLICE},
                                {"Play", "roboto", Color::black(),false, false, [this]() {
                                    AT_INFO("Play button clicked!");
                                }, nullptr, nullptr, Color(255,0,0), Color(199, 2, 2), Color(89, 1, 1)});

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
        auto width = GameManager::getWindowRef()->getWidth();
        auto height = GameManager::getWindowRef()->getHeight();

        GameManager::getWindowRef()->setWindowSize(width * 3/4, height * 2/3);
        GameManager::getWindowRef()->centerWindow();
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
        static bool wireFrame = false;
        ImGui::Checkbox("Wireframe", &wireFrame);

        if (wireFrame) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        } else {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
        ImGui::End();

        //Button::update(registry, camera);
        uiSystem.update(deltaTime, registry, camera);
        renderSystem.update(deltaTime, registry);
    }


    void onRender(int screenWidth, int screenHeight) override {
        /*const glm::vec2 topLeft = camera.screenToWorld({0.0f, 0.0f});
        const glm::vec2 bottomRight = camera.screenToWorld({static_cast<float>(screenWidth), static_cast<float>(screenHeight)});

        const float worldWidth = bottomRight.x - topLeft.x;
        const float worldHeight = topLeft.y - bottomRight.y;

        auto &backgroundTransform = registry.get<TransformComponent>(this->background);
        backgroundTransform.scale = glm::vec2(worldWidth, worldHeight);*/

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
