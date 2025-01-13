#include "GameModeScene.hpp"

#include <imgui.h>

#include "core/GameManager.hpp"
#include "network/ClientNetworkService.hpp"
#include "renderer/Color.hpp"
#include "window/Window.hpp"


void GameModeScene::onCreate() {
    ResourceManager::load<Font>("pixelify", "assets/fonts/PixelifySans-Medium.ttf");
    ResourceManager::load<Font>("roboto", "assets/fonts/Roboto-Light.ttf");
    ResourceManager::load<Font>("minecraft", "assets/fonts/Minecraft.ttf");
    ResourceManager::load<Font>("inter", "assets/fonts/InterVariable.ttf");
    ResourceManager::load<Font>("thaleah", "assets/fonts/ThaleahFat.ttf");
    ResourceManager::loadFromDirectory<Texture>("assets/textures", nullptr, ".png", ".jpg");
}

void GameModeScene::disableButtons() {
    registry.emplace_or_replace<DisabledComponent>(hexDuelButton);
    registry.emplace_or_replace<DisabledComponent>(hexArenaButton);
}

void GameModeScene::enableButtons() {
    registry.remove<DisabledComponent>(hexDuelButton);
    registry.remove<DisabledComponent>(hexArenaButton);
}

void GameModeScene::onStart() {
    const auto &windowRef = GameManager::getWindowRef();

    int newHeight = static_cast<int>(windowRef->getMonitorSize().second * 0.75);
    int newWidth = (newHeight * 16) / 9;

    windowRef->setWindowSize(newWidth, newHeight);
    windowRef->centerWindow();
    windowRef->setWindowStyle(Window::Style::UNDECORATED);

    auto onBackButtonCallback = []() {
        GameManager::changeScene("MenuScene");
    };

    auto findingButton = [this](const std::string &buttonText, const std::string &actionText, glm::vec3 position) {
        return Actors::createButton(
            this->registry,
            {position, 0.0f, glm::vec2(600.0f, 200.0f)},
            {"panel-transparent-border-010", RenderComponent::defaultTexCoords(), Color::white(), true, RENDERER_NINE_SLICE},
            {actionText, "thaleah", Color::black(), Color::white(), Color::white(), Color::black(), false, false, Color::white(), Color(109, 52, 133), Color(54, 26, 66), Color::gray(), false, nullptr, nullptr, nullptr}
        );
    };

    auto stopButton = [this](glm::vec3 position) {
        return Actors::createButton(
            this->registry,
            {position, 0.0f, glm::vec2(100.0f, 100.0f)},
            {"panel-transparent-border-010", RenderComponent::defaultTexCoords(), Color::white(), true, RENDERER_NINE_SLICE},
            {"X", "thaleah", Color::black(), Color::white(), Color::white(), Color::black(), false, false, Color::white(), Color(1.0f, 0.0f, 0.0f, 1.0f), Color(1.0f, 0.0f, 0.0f, 1.0f), Color::gray(), false, [this]() {
                // Re-enable Hex Duel and Hex Arena buttons
                enableButtons();
            }, nullptr, nullptr}
        );
    };

    auto onHalfMatchButton = [this, findingButton, stopButton]() {
        // Disable both buttons if they exist
        if (!hexDuelpressed) {
            disableButtons();
            findDuelButton = findingButton("Find Duel", "Hex Duel", glm::vec3(-1100.0f, 800.0f, 0.0f));
            stopFindingButton = stopButton(glm::vec3(-500.0f, 800.0f, 0.0f));
            hexDuelpressed = true;
        }
    };

    auto onFullMatchButton = [this, findingButton, stopButton]() {
        // Disable both buttons if they exist
        if (!hexArenapressed) {
            disableButtons();
            findArenaButton = findingButton("Find Arena", "Hex Arena", glm::vec3(-1100.0f, 800.0f, 0.0f));
            stopFindingButton = stopButton(glm::vec3(-500.0f, 800.0f, 0.0f));
            hexArenapressed = true;
        }
    };

    this->sideRect = Actors::createStaticProp(this->registry,
                                              {glm::vec3(-1100.0f, 0.0f, 1.0f), 0.0f, glm::vec2(1700.0f, 2400.0f)},
                                              {"", RenderComponent::defaultTexCoords(), Color(22, 22, 22, 200), true});

    // Background
    this->background = Actors::createStaticProp(this->registry,
                                                {glm::vec3(0.0f, 0.0f, 1.0f), 0.0f, glm::vec2(1000.0f, 500.0f)},
                                                {"background01", RenderComponent::defaultTexCoords(), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), true});

    // Title
    this->title = Actors::createStaticProp(this->registry,
                                           {glm::vec3(-1135.0f, 470.0f, 0.0f), 0.0f, glm::vec2(15.0f, 30.0f)},
                                           {"thaleah", "Select game mode", true, Color::white()});

    // 1v1 Button
    this->hexDuelButton = Actors::createButton(this->registry,
                                               {glm::vec3(-1100.0f, 100.0f, 0.0f), 0.0f, glm::vec2(900.0f, 200.0f)},
                                               {"panel-transparent-border-010", RenderComponent::defaultTexCoords(), Color::white(), true, RENDERER_NINE_SLICE},
                                               {"Hex Duel", "thaleah", Color::black(), Color::white(), Color::white(), Color::black(), false, false, Color::white(), Color(109, 52, 133), Color(54, 26, 66), Color::gray(), false, onHalfMatchButton, nullptr, nullptr});

    this->hexDuelDescription = Actors::createStaticProp(this->registry,
                                                        {glm::vec3(-1100.0f, -100.0f, 0.0f), 0.0f, glm::vec2(10.0f, 10.0f)},
                                                        {"thaleah", "1v1", true, Color::white()});

    // 1v1v1v1 Button
    this->hexArenaButton = Actors::createButton(this->registry,
                                                {glm::vec3(-1100.0f, -300.0f, 0.0f), 0.0f, glm::vec2(900.0f, 200.0f)},
                                                {"panel-transparent-border-010", RenderComponent::defaultTexCoords(), Color::white(), true, RENDERER_NINE_SLICE},
                                                        {"Hex Arena", "thaleah", Color::black(), Color::white(), Color::white(), Color::black(), false, false, Color::white(), Color(109, 52, 133), Color(54, 26, 66), Color::gray(), false, onFullMatchButton, nullptr, nullptr});

    this->hexArenaDescription = Actors::createStaticProp(this->registry,
                                                         {glm::vec3(-1100.0f, -500.0f, 0.0f), 0.0f, glm::vec2(10.0f, 10.0f)},
                                                         {"thaleah", "1v1v1v1", true, Color::white()});

    // Back Button
    this->backButton = Actors::createButton(this->registry,
                                            {glm::vec3(-1500.0f, -800.0f, 0.0f), 0.0f, glm::vec2(500.0f, 150.0f)},
                                            {"panel-transparent-border-010", RenderComponent::defaultTexCoords(), Color::white(), true, RENDERER_NINE_SLICE},
                                                {"Back", "thaleah", Color::black(), Color::white(), Color::white(), Color::black(), false, false, Color::white(), Color(109, 52, 133), Color(54, 26, 66), Color::gray(), false, onBackButtonCallback, nullptr, nullptr});
}


void GameModeScene::onUpdate(float deltaTime) {
    uiSystem.update(deltaTime, registry, camera);
    renderSystem.update(registry);
}

void GameModeScene::onRender(int screenWidth, int screenHeight) {
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

void GameModeScene::onDestroy() {
    ResourceManager::clearAll();
}
