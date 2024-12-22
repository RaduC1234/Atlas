//
// Created by Radu on 21-Dec-24.
//
#include "MenuScene.hpp"

#include <imgui.h>

#include "core/GameManager.hpp"
#include "network/ClientNetworkService.hpp"
#include "renderer/Color.hpp"
#include "window/Window.hpp"

void MenuScene::onCreate() {
    ResourceManager::load<Font>("pixelify", "assets/fonts/PixelifySans-Medium.ttf");
    ResourceManager::load<Font>("roboto", "assets/fonts/Roboto-Light.ttf");
    ResourceManager::load<Font>("minecraft", "assets/fonts/Minecraft.ttf");
    ResourceManager::load<Font>("inter", "assets/fonts/InterVariable.ttf");
    ResourceManager::load<Font>("thaleah", "assets/fonts/ThaleahFat.ttf");

    ResourceManager::loadFromDirectory<Texture>("assets/textures", nullptr, ".png", ".jpg");
}

void MenuScene::onStart() {
    const auto &windowRef = GameManager::getWindowRef();

    int newHeight = static_cast<int>(windowRef->getMonitorSize().second * 0.75);
    int newWidth = (newHeight * 16) / 9;

    windowRef->setWindowSize(newWidth, newHeight);
    windowRef->centerWindow();
    windowRef->setWindowStyle(Window::Style::UNDECORATED);

    auto onPlayButtonCallback = []() {
        GameManager::changeScene("GameModeScene");
    };


    this->sideRect = Actors::createStaticProp(this->registry,
                                          {glm::vec3(-1100.0f, -300.0f, 1.0f), 0.0f, glm::vec2(1060.0f, 1000.0f)},
                                          {"", RenderComponent::defaultTexCoords(), Color(22,22,22,210), true}
    );

    this->background = Actors::createStaticProp(this->registry, // here
                                                {glm::vec3(0.0f, 0.0f, 1.0f), 0.0f, glm::vec2(1000.0f, 500.0f)},
                                                {"background01", RenderComponent::defaultTexCoords(), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), true}
    );

    this->gameTitle = Actors::createStaticProp(this->registry,
                                             {glm::vec3(-1100.0f, 600.0f, 0.0f), 0.0f, glm::vec2(900.0f, 900.0f)},
                                    {"iconAndLogo", RenderComponent::defaultTexCoords(), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), true}
                                    );


    this->playButton = Actors::createButton(this->registry,
    {glm::vec3(-1100.0f, 0.0f, 0.0f), 0.0f, glm::vec2(900.0f, 200.0f)},
     {"panel-transparent-border-010", RenderComponent::defaultTexCoords(), Color::white(), true, RENDERER_NINE_SLICE},
     {"Play", "thaleah",  Color::black(), Color::white(), Color::white(), false, false, Color::white(), Color(109, 52, 133), Color(54, 26, 66),
        onPlayButtonCallback, nullptr, nullptr});

    this->optionsButton = Actors::createButton(this->registry,
                 {glm::vec3(-1100.0f, -300.0f, 0.0f), 0.0f, glm::vec2(900.0f, 200.0f)},  // Positioned in the left half and horizontally centered
                 {"panel-transparent-border-010", RenderComponent::defaultTexCoords(), Color::white(), true, RENDERER_NINE_SLICE},
                 {"Options", "thaleah",  Color::black(), Color::white(), Color::white(), false, false, Color::white(), Color(109, 52, 133), Color(54, 26, 66),
                     [this]() {
                         AT_INFO("Options button clicked!");
                     }, nullptr, nullptr});

    this->quitButton = Actors::createButton(this->registry,
                 {glm::vec3(-1100.0f, -600.0f, 0.0f), 0.0f, glm::vec2(900.0f, 200.0f)},  // Positioned in the left half and horizontally centered
                 {"panel-transparent-border-010", RenderComponent::defaultTexCoords(), Color::white(), true, RENDERER_NINE_SLICE},
                 {"Quit", "thaleah",  Color::black(), Color::white(), Color::white(), false, false, Color::white(), Color(109, 52, 133), Color(54, 26, 66),
                     [this]() {
                         AT_INFO("Quit button clicked!");
                     }, nullptr, nullptr});
}

void MenuScene::onUpdate(float deltaTime) {
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

void MenuScene::onRender(int screenWidth, int screenHeight) {

    const float baseWidth = 1920.0f;
    const float baseHeight = 1080.0f;
    const float baseZoom = 0.5f;

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

void MenuScene::onDestroy() {
    ResourceManager::clearAll();
}

