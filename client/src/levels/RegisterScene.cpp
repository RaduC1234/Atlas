#include "RegisterScene.hpp"

#include <imgui.h>

#include "core/GameManager.hpp"
#include "network/ClientNetworkService.hpp"
#include "renderer/Color.hpp"
#include "resource/ResourceManager.hpp"
#include "window/Window.hpp"

void RegisterScene::onCreate() {
    ResourceManager::load<Font>("pixelify", "assets/fonts/PixelifySans-Medium.ttf");
    ResourceManager::load<Font>("roboto", "assets/fonts/Roboto-Light.ttf");
    ResourceManager::load<Font>("minecraft", "assets/fonts/Minecraft.ttf");
    ResourceManager::load<Font>("inter", "assets/fonts/InterVariable.ttf");
    ResourceManager::load<Font>("thaleah", "assets/fonts/ThaleahFat.ttf");

    ResourceManager::loadFromDirectory<Texture>("assets/textures", nullptr, ".png", ".jpg");
}

void RegisterScene::showErrorMessage(const std::string& message) {
    // Remove existing error message if it exists
    if (registry.valid(errorMessage)) {
        registry.destroy(errorMessage);
    }

    // Create new error message
    this->errorMessage = Actors::createStaticProp(this->registry,
        {glm::vec3(-1090.0f, -350.0f, 0.0f),
         3.0f,
         glm::vec2(5.0f, 15.0f)},
        {"thaleah",
         message,
         true,
         Color(255, 77, 77, 255)}  // Red color for error
    );

    errorMessageTimer = ERROR_MESSAGE_DURATION;
}

void RegisterScene::onStart() {
    const auto &windowRef = GameManager::getWindowRef();

    int newHeight = static_cast<int>(windowRef->getMonitorSize().second * 0.75);
    int newWidth = (newHeight * 16) / 9;

    windowRef->setWindowSize(newWidth, newHeight);
    windowRef->centerWindow();
    windowRef->setWindowStyle(Window::Style::UNDECORATED);

    auto onBackButtonCallback = []() {
        GameManager::changeScene("LoginScene");
    };

    auto registerCallback = [this]() {
        if (registry.valid(usernameTextBox) && registry.valid(passwordTextBox)) {
            const std::string& username = registry.get<TextboxComponent>(this->usernameTextBox).text;
            const std::string& password = registry.get<TextboxComponent>(this->passwordTextBox).text;

            // Validate input fields
            if (username.empty() || password.empty()) {
                showErrorMessage("Username and password cannot be empty");
                return;
            }

            try {
                if (ClientNetworkService::reg(username, password)) {
                    AT_INFO("Registered successfully");
                    GameManager::changeScene("LoginScene"); // Redirect to login after successful registration
                } else {
                    showErrorMessage("Registration failed. Please try again.");
                }
            } catch (const std::exception& e) {
                showErrorMessage(e.what());
            }
        }
    };

    this->gamelogo = Actors::createStaticProp(this->registry,
                                               {glm::vec3(-1100.0f, 750.0f, 0.0f), 0.0f, glm::vec2(400.0f, 400.0f)},
                                               {"iconAndLogo", RenderComponent::defaultTexCoords(), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), true}
    );

    this->sideRect = Actors::createStaticProp(this->registry,
                                              {glm::vec3(-1100.0f, 0.0f, 1.0f), 0.0f, glm::vec2(1700.0f, 2400.0f)},
                                              {"", RenderComponent::defaultTexCoords(), Color(22,22,22,200), true}
    );

    // Main background
    this->background = Actors::createStaticProp(this->registry,
                                                {glm::vec3(0.0f, 0.0f, 2.0f), 0.0f, glm::vec2(1000.0f, 500.0f)},
                                                {"background01", RenderComponent::defaultTexCoords(), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), true}
    );

    // Title
    this->title = Actors::createStaticProp(this->registry,
                                           {glm::vec3(-1135.0f, 300.0f, 0.0f), 3.0f, glm::vec2(15.0f, 30.0f)},
                                           {"thaleah", "Register", true, Color::white()}
    );

    // Username textbox
    this->usernameTextBox = Actors::createTextbox(this->registry,
                                                  {glm::vec3(-1090.0f, 180.0f, 0.0f), 0.0f, glm::vec2(1300.0f, 200.0f)},
                                                  {"panel-border-013", RenderComponent::defaultTexCoords(), Color::white(), true, RENDERER_NINE_SLICE},
                                                  {"", "thaleah", Color::white(), 12}
    );

    // Password textbox
    this->passwordTextBox = Actors::createTextbox(this->registry,
                                                  {glm::vec3(-1090.0f, -170.0f, 0.0f), 0.0f, glm::vec2(1300.0f, 200.0f)},
                                                  {"panel-border-013", RenderComponent::defaultTexCoords(), Color::white(), true, RENDERER_NINE_SLICE},
                                                  {"", "thaleah", Color::white(), 12, 0, false,false,true}
    );

    // Register button
    this->registerButton = Actors::createButton(this->registry,
        {glm::vec3(-1090.0f, -520.0f, 0.0f), 0.0f, glm::vec2(1000.0f, 250.0f)},
        {"panel-transparent-border-010", RenderComponent::defaultTexCoords(), Color::white(), true, RENDERER_NINE_SLICE},
        {"Register", "thaleah", Color::black(), Color::white(), Color::white(), Color::black(), false, false, Color::white(), Color(109, 52, 133), Color(54, 26, 66), Color::gray(), false,
            registerCallback, nullptr, nullptr}
    );

    this->backButton = Actors::createButton(this->registry,
            {glm::vec3(-1500.0f, -800.0f, 0.0f), 0.0f, glm::vec2(500.0f, 150.0f)},
            {"panel-transparent-border-010", RenderComponent::defaultTexCoords(), Color::white(), true, RENDERER_NINE_SLICE},
            {"Back", "thaleah", Color::black(), Color::white(), Color::white(), Color::black(), false, false, Color::white(), Color(109, 52, 133), Color(54, 26, 66), Color::gray(), false,
                onBackButtonCallback, nullptr, nullptr});
}

void RegisterScene::onUpdate(float deltaTime) {
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

    if (registry.valid(errorMessage)) {
        errorMessageTimer -= deltaTime;
        if (errorMessageTimer <= 0) {
            registry.destroy(errorMessage);
        } else {
            // Fade out the error message
            auto& renderComp = registry.get<RenderComponent>(errorMessage);
            float alpha = std::min(1.0f, errorMessageTimer / ERROR_MESSAGE_DURATION);
            renderComp.color.a = static_cast<unsigned char>(alpha * 255);
        }
    }

    RenderManager::drawQuad(glm::vec3(-1100.0f, 750.0f, 0.0f), glm::vec2(400.0f, 400.0f), Color::white(), ResourceManager::get<Texture>("iconAndLogo"), true);
    uiSystem.update(deltaTime, registry, camera);
    renderSystem.update(registry);
}

void RegisterScene::onRender(int screenWidth, int screenHeight) {

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

void RegisterScene::onDestroy() {
    ResourceManager::clearAll();
}
