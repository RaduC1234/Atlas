#include "GameModeScene.hpp"

#include <imgui.h>

#include "core/GameManager.hpp"
#include "network/ClientNetworkService.hpp"
#include "renderer/Color.hpp"
#include "renderer/Font.hpp"
#include "resource/ResourceManager.hpp"
#include "window/Window.hpp"

void GameModeScene::onCreate() {
    ResourceManager::load<Font>("pixelify", "assets/fonts/PixelifySans-Medium.ttf");
    ResourceManager::load<Font>("roboto", "assets/fonts/Roboto-Light.ttf");
    ResourceManager::load<Font>("minecraft", "assets/fonts/Minecraft.ttf");
    ResourceManager::load<Font>("inter", "assets/fonts/InterVariable.ttf");
    ResourceManager::load<Font>("thaleah", "assets/fonts/ThaleahFat.ttf");
    ResourceManager::loadFromDirectory<Texture>("assets/textures", nullptr, ".png", ".jpg");
}

void GameModeScene::enableLeaveQueueButton(std::function<void()> onClick) {
    if (registry.valid(leaveQueueButton)) {
        registry.remove<DisabledComponent>(leaveQueueButton);
        auto& button = registry.get<ButtonComponent>(leaveQueueButton);
        button.isDisabled = false;
        button.onClick = onClick;

        auto& render = registry.get<RenderComponent>(leaveQueueButton);
        render.color = Color::white(); // Enabled color
    }
}

void GameModeScene::disableLeaveQueueButton() {
    if (registry.valid(leaveQueueButton)) {
        registry.emplace_or_replace<DisabledComponent>(leaveQueueButton, true);
        auto& button = registry.get<ButtonComponent>(leaveQueueButton);
        button.isDisabled = true;

        auto& render = registry.get<RenderComponent>(leaveQueueButton);
        render.color = Color::gray(); // Disabled color
    }
}

void GameModeScene::enableButtons() {
    if (registry.valid(hexDuelButton)) {
        registry.remove<DisabledComponent>(hexDuelButton);
        auto& button = registry.get<ButtonComponent>(hexDuelButton);
        button.isDisabled = false;

        auto& render = registry.get<RenderComponent>(hexDuelButton);
        render.color = Color::white(); // Normal color
    }

    if (registry.valid(hexArenaButton)) {
        registry.remove<DisabledComponent>(hexArenaButton);
        auto& button = registry.get<ButtonComponent>(hexArenaButton);
        button.isDisabled = false;

        auto& render = registry.get<RenderComponent>(hexArenaButton);
        render.color = Color::white(); // Normal color
    }
}

void GameModeScene::disableButtons() {
    if (registry.valid(hexDuelButton)) {
        registry.emplace_or_replace<DisabledComponent>(hexDuelButton, true);
        auto& button = registry.get<ButtonComponent>(hexDuelButton);
        button.isDisabled = true;

        auto& render = registry.get<RenderComponent>(hexDuelButton);
        render.color = Color::gray(); // Disabled color
    }

    if (registry.valid(hexArenaButton)) {
        registry.emplace_or_replace<DisabledComponent>(hexArenaButton, true);
        auto& button = registry.get<ButtonComponent>(hexArenaButton);
        button.isDisabled = true;

        auto& render = registry.get<RenderComponent>(hexArenaButton);
        render.color = Color::gray(); // Disabled color
    }
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

    auto leaveCurrentQueue = [this]() {
        if (hexDuelpressed || hexArenapressed) {
            AT_INFO("Leaving current queue...");
            ClientNetworkService::leaveMatchmaking();

            // Reset state
            hexDuelpressed = false;
            hexArenapressed = false;

            // Disable the "Leave Queue" button
            disableLeaveQueueButton();

            // Re-enable Hex Duel and Hex Arena buttons
            enableButtons();
        }
    };

    // Create Leave Queue button (initially disabled)
    this->leaveQueueButton = Actors::createButton(
        registry,
        {glm::vec3(-1100.0f, 800.0f, 0.0f), 0.0f, glm::vec2(1150.0f, 250.0f)},
        {"panel-transparent-border-010", RenderComponent::defaultTexCoords(), Color::gray(), true, RENDERER_NINE_SLICE},
        {"Leave Queue", "thaleah", Color::black(), Color::white(), Color::white(), Color::black(), false, false, Color::white(), Color(109, 52, 133), Color(54, 26, 66), Color::gray(), true, nullptr, nullptr, nullptr}
    );
    registry.emplace<DisabledComponent>(leaveQueueButton, true); // Start disabled

    // Enable the "Leave Queue" button when a queue is joined
    auto enableLeaveQueueButton = [this, leaveCurrentQueue]() {
        registry.remove<DisabledComponent>(leaveQueueButton);
        auto& button = registry.get<ButtonComponent>(leaveQueueButton);
        button.isDisabled = false;
        button.onClick = leaveCurrentQueue;

        auto& render = registry.get<RenderComponent>(leaveQueueButton);
        render.color = Color::white(); // Set to enabled color
    };

    // Create Hex Duel button
    auto onHexDuelButton = [this, enableLeaveQueueButton]() {
        try {
            AT_INFO("Joining Hex Duel queue...");
            ClientNetworkService::joinMatchmaking(ClientNetworkService::GameMode::HEX_DUEL);
            disableButtons();
            hexDuelpressed = true;

            enableLeaveQueueButton(); // Enable Leave Queue button
        } catch (const std::exception &e) {
            AT_ERROR("Failed to join Hex Duel matchmaking: {}", e.what());
            enableButtons();
        }
    };

    // Create Hex Arena button
    auto onHexArenaButton = [this, enableLeaveQueueButton]() {
        try {
            AT_INFO("Joining Hex Arena queue...");
            ClientNetworkService::joinMatchmaking(ClientNetworkService::GameMode::HEX_ARENA);
            disableButtons();
            hexArenapressed = true;

            enableLeaveQueueButton(); // Enable Leave Queue button
        } catch (const std::exception &e) {
            AT_ERROR("Failed to join Hex Arena matchmaking: {}", e.what());
            enableButtons();
        }
    };

    // Create Hex Duel button
    hexDuelButton = Actors::createButton(
        registry,
        {glm::vec3(-1100.0f, 100.0f, 0.0f), 0.0f, glm::vec2(900.0f, 200.0f)},
        {"panel-transparent-border-010", RenderComponent::defaultTexCoords(), Color::white(), true, RENDERER_NINE_SLICE},
        {"Hex Duel", "thaleah", Color::black(), Color::white(), Color::white(), Color::black(), false, false, Color::white(), Color(109, 52, 133), Color(54, 26, 66), Color::gray(), false, onHexDuelButton, nullptr, nullptr}
    );

    // Create Hex Arena button
    hexArenaButton = Actors::createButton(
        registry,
        {glm::vec3(-1100.0f, -300.0f, 0.0f), 0.0f, glm::vec2(900.0f, 200.0f)},
        {"panel-transparent-border-010", RenderComponent::defaultTexCoords(), Color::white(), true, RENDERER_NINE_SLICE},
        {"Hex Arena", "thaleah", Color::black(), Color::white(), Color::white(), Color::black(), false, false, Color::white(), Color(109, 52, 133), Color(54, 26, 66), Color::gray(), false, onHexArenaButton, nullptr, nullptr}
    );

    // Create Title and other elements
    this->title = Actors::createStaticProp(this->registry,
                                           {glm::vec3(-1135.0f, 320.0f, 0.0f), 0.0f, glm::vec2(15.0f, 30.0f)},
                                           {"thaleah", "Select game mode", true, Color::white()});

    this->sideRect = Actors::createStaticProp(this->registry,
                                              {glm::vec3(-1100.0f, 0.0f, 1.0f), 0.0f, glm::vec2(1700.0f, 2400.0f)},
                                              {"", RenderComponent::defaultTexCoords(), Color(22, 22, 22, 200), true});

    // Background
    this->background = Actors::createStaticProp(this->registry,
                                                {glm::vec3(0.0f, 0.0f, 1.0f), 0.0f, glm::vec2(1000.0f, 500.0f)},
                                                {"background01", RenderComponent::defaultTexCoords(), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), true});

    // Title
    this->title = Actors::createStaticProp(this->registry,
                                           {glm::vec3(-1135.0f, 320.0f, 0.0f), 0.0f, glm::vec2(15.0f, 30.0f)},
                                           {"thaleah", "Select game mode", true, Color::white()});

    // 1v1 Button
    this->hexDuelButton = Actors::createButton(this->registry,
                                               {glm::vec3(-1100.0f, 100.0f, 0.0f), 0.0f, glm::vec2(900.0f, 200.0f)},
                                               {"panel-transparent-border-010", RenderComponent::defaultTexCoords(), Color::white(), true, RENDERER_NINE_SLICE},
                                               {"Hex Duel", "thaleah", Color::black(), Color::white(), Color::white(), Color::black(), false, false, Color::white(), Color(109, 52, 133), Color(54, 26, 66), Color::gray(), false, onHexDuelButton, nullptr, nullptr});

    this->hexDuelDescription = Actors::createStaticProp(this->registry,
                                                        {glm::vec3(-1100.0f, -100.0f, 0.0f), 0.0f, glm::vec2(10.0f, 10.0f)},
                                                        {"thaleah", "1v1", true, Color::white()});

    // 1v1v1v1 Button
    this->hexArenaButton = Actors::createButton(this->registry,
                                                {glm::vec3(-1100.0f, -300.0f, 0.0f), 0.0f, glm::vec2(900.0f, 200.0f)},
                                                {"panel-transparent-border-010", RenderComponent::defaultTexCoords(), Color::white(), true, RENDERER_NINE_SLICE},
                                                        {"Hex Arena", "thaleah", Color::black(), Color::white(), Color::white(), Color::black(), false, false, Color::white(), Color(109, 52, 133), Color(54, 26, 66), Color::gray(), false, onHexArenaButton, nullptr, nullptr});

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
    static float matchCheckTimer = 0.0f;
    matchCheckTimer += deltaTime;

    // Check match status every second if in queue
    if ((hexDuelpressed || hexArenapressed) && matchCheckTimer >= 1.0f) {
        try {
            if (ClientNetworkService::checkMatchStatus()) {
                // Store the player's role in the match
                auto playerId = ClientNetworkService::getCurrentPlayerId();
                auto matchId = ClientNetworkService::getCurrentMatchId();

                // Set the window to fullscreen and undecorated
                auto *windowRef = GameManager::getWindowRef();
                auto monitorSize = windowRef->getMonitorSize();

                windowRef->setWindowSize(monitorSize.first, monitorSize.second);
                windowRef->setWindowStyle(Window::Style::DECORATED);

                // Store match data and change scene
                GameManager::setMatchData(matchId, playerId);
                GameManager::changeScene("MatchScene");
                return;
            }
        } catch (const std::exception& e) {
            AT_ERROR("Failed to check match status: {}", e.what());
            enableButtons(); // Re-enable buttons on error
            hexDuelpressed = false;
            hexArenapressed = false;
        }
        matchCheckTimer = 0.0f;
    }

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
    if (hexDuelpressed || hexArenapressed) {
        try {
            ClientNetworkService::leaveMatchmaking();
        } catch (const std::exception& e) {
            AT_ERROR("Failed to leave matchmaking: {}", e.what());
        }
    }
    ResourceManager::clearAll();
}