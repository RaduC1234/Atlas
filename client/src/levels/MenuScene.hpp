#pragma once

#include <imgui.h>
#include <Atlas.hpp>
#include "system/PawnSystem.hpp"
#include "system/RenderSystem.hpp"
#include "system/UISystem.hpp"
#include "network/ClientNetworkService.hpp"
#include "renderer/Color.hpp"

class MenuScene : public Scene {
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
                                              {glm::vec3(-1100.0f, 0.0f, 1.0f), 0.0f, glm::vec2(1600.0f, 2030.0f)},
                                              {"", RenderComponent::defaultTexCoords(), Color(22,22,22), true}
        );

        this->background = Actors::createStaticProp(this->registry, // here
                                                    {glm::vec3(0.0f, 0.0f, 1.0f), 0.0f, glm::vec2(1000.0f, 500.0f)},
                                                    {"background01", RenderComponent::defaultTexCoords(), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), true}
        );

        this->title = Actors::createStaticProp(this->registry,
                                        {glm::vec3(-1150.0f, 600.0f, 0.0f), 0.0f, glm::vec2(25.0f, 15.0f)},
                                        {"thaleah", "Hex&Chaos",true,Color(63,47, 102)}
                );

        this->playButton = Actors::createButton(this->registry,
        {glm::vec3(-1100.0f, 200.0f, 0.0f), 0.0f, glm::vec2(900.0f, 200.0f)},
         {"panel-transparent-border-010", RenderComponent::defaultTexCoords(), Color(63,47,102), true, RENDERER_NINE_SLICE},
         {"Play", "roboto", Color::black(), false, false, [this]() {
             AT_INFO("Play button clicked!");
         }, nullptr, nullptr, Color(63, 47, 102), Color(44, 33, 71), Color(38, 28, 61)});

        this->optionsButton = Actors::createButton(this->registry,
                     {glm::vec3(-1100.0f, -100.0f, 0.0f), 0.0f, glm::vec2(900.0f, 200.0f)},  // Positioned in the left half and horizontally centered
                     {"panel-transparent-border-010", RenderComponent::defaultTexCoords(), Color(63,47,102), true, RENDERER_NINE_SLICE},
                     {"Options", "roboto", Color::black(), false, false, [this]() {
                         AT_INFO("Options button clicked!");
                     }, nullptr, nullptr, Color(63, 47, 102), Color(44, 33, 71), Color(38, 28, 61)});

        this->quitButton = Actors::createButton(this->registry,
                     {glm::vec3(-1100.0f, -400.0f, 0.0f), 0.0f, glm::vec2(900.0f, 200.0f)},  // Positioned in the left half and horizontally centered
                     {"panel-transparent-border-010", RenderComponent::defaultTexCoords(), Color(63,47,102), true, RENDERER_NINE_SLICE},
                     {"Quit", "roboto", Color::black(), false, false, [this]() {
                         AT_INFO("Quit button clicked!");
                     }, nullptr, nullptr, Color(63, 47, 102), Color(44, 33, 71), Color(38, 28, 61)});

        this->loginButton = Actors::createButton(this->registry,
             {glm::vec3(-1100.0f, -700.0f, 0.0f), 0.0f, glm::vec2(900.0f, 200.0f)},  // Positioned in the left half and horizontally centered
             {"panel-transparent-border-010", RenderComponent::defaultTexCoords(), Color(63,47,102), true, RENDERER_NINE_SLICE},
             {"Login", "roboto", Color::black(), false, false, [this]() {
                 AT_INFO("Login button clicked!");
             }, nullptr, nullptr, Color(63, 47, 102), Color(44, 33, 71), Color(38, 28, 61)});

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
    Actor playButton;
    Actor optionsButton;
    Actor quitButton;
    Actor loginButton;

};
