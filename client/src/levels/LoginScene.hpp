#pragma once

#include <imgui.h>
#include <Atlas.hpp>
#include "system/PawnSystem.hpp"
#include "system/RenderSystem.hpp"
#include "system/UISystem.hpp"
#include "network/ClientNetworkService.hpp"

class LoginScene : public Scene {
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
        this->gameTitle = Actors::createStaticProp(this->registry,
                                                 {glm::vec3(-890.0f, 700.0f, 0.0f), 0.0f, glm::vec2(500.0f, 500.0f)},
                                        {"logo6", RenderComponent::defaultTexCoords(), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), true}
                                        );

        this->sideRect = Actors::createStaticProp(this->registry,
                                                {glm::vec3(-900.0f, 0.0f, 4.0f), 0.0f, glm::vec2(1700.0f, 2030.0f)},
                                                {"", RenderComponent::defaultTexCoords(), Color(22,22,22), true}
        );

        // Main background
        this->background = Actors::createStaticProp(this->registry,
                                                  {glm::vec3(0.0f, 0.0f, 4.0f), 0.0f, glm::vec2(1000.0f, 500.0f)},
                                                  {"background01", RenderComponent::defaultTexCoords(), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), true}
        );

        // Title
        this->title = Actors::createStaticProp(this->registry,
                                             {glm::vec3(-935.0f, 270.0f, 0.0f), 3.0f, glm::vec2(15.0f, 30.0f)},
                                    {"thaleah", "Sign in", true, Color::white()}
        );

        // Username textbox
        this->usernameTextBox = Actors::createTextbox(this->registry,
                                                    {glm::vec3(-890.0f, 150.0f, 0.0f), 0.0f, glm::vec2(1300.0f, 200.0f)},
                                                    {"panel-border-013", RenderComponent::defaultTexCoords(), Color::white(), true, RENDERER_NINE_SLICE},
                                                    {"", "thaleah", Color::white(), 12}
        );

        // Password textbox
        this->passwordTextBox = Actors::createTextbox(this->registry,
                                                    {glm::vec3(-890.0f, -200.0f, 0.0f), 0.0f, glm::vec2(1300.0f, 200.0f)},
                                                    {"panel-border-013", RenderComponent::defaultTexCoords(), Color::white(), true, RENDERER_NINE_SLICE},
                                                    {"", "thaleah", Color::white(), 12}
        );

        // Login button
        this->loginButton = Actors::createButton(this->registry,
                                    {glm::vec3(-890.0f, -550.0f, 0.0f), 0.0f, glm::vec2(1000.0f, 250.0f)},
                                                {"panel-transparent-border-010", RenderComponent::defaultTexCoords(), Color(255,255,255), true, RENDERER_NINE_SLICE},
                                      {"Hex in", "thaleah", Color::black(), false, false,[this]() {
                if (registry.valid(usernameTextBox) && registry.valid(passwordTextBox)) {
                    const std::string& username = registry.get<TextboxComponent>(this->usernameTextBox).text;
                    const std::string& password = registry.get<TextboxComponent>(this->passwordTextBox).text;

                    if (ClientNetworkService::login(username, password)) {
                        std::cout << "Login successful!" << std::endl;
                    } else {
                        std::cerr << "Login failed! Please check your credentials." << std::endl;
                    }
                }
            }, nullptr, nullptr, Color(255,255,255), Color(227, 225, 225), Color(176, 174, 174)}
        );

        // Register button
        this->registerButton = Actors::createButton(this->registry,
            {glm::vec3(-890.0f, -820.0f, 0.0f), 0.0f, glm::vec2(600.0f, 150.0f)},
            {"panel-transparent-border-010", RenderComponent::defaultTexCoords(), Color(255,255,255), true, RENDERER_NINE_SLICE},
            {"Register", "thaleah", Color::black(), false, false, [this]() {},
            nullptr, nullptr, Color(255,255,255), Color(227, 225, 225), Color(176, 174, 174)}
        );
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
    Camera camera{{0.0f, 0.0f}, 0.25f};
    Registry registry;

    RenderSystem renderSystem;
    PawnSystem pawnSystem;
    UISystem uiSystem;

    Actor gameTitle;
    Actor background;
    Actor sideRect;
    Actor usernameTextBox;
    Actor passwordTextBox;
    Actor loginButton;
    Actor registerButton;
    Actor title;

};