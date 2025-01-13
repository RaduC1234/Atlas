#include "LoginScene.hpp"

#include <imgui.h>

#include "core/GameManager.hpp"
#include "network/ClientNetworkService.hpp"
#include "renderer/Color.hpp"
#include "window/Window.hpp"


void LoginScene::onCreate() {
    ResourceManager::load<Font>("pixelify", "assets/fonts/PixelifySans-Medium.ttf");
    ResourceManager::load<Font>("roboto", "assets/fonts/Roboto-Light.ttf");
    ResourceManager::load<Font>("minecraft", "assets/fonts/Minecraft.ttf");
    ResourceManager::load<Font>("inter", "assets/fonts/InterVariable.ttf");
    ResourceManager::load<Font>("thaleah", "assets/fonts/ThaleahFat.ttf");

    ResourceManager::loadFromDirectory<Texture>("assets/textures", nullptr, ".png", ".jpg");
}

void LoginScene::onStart() {
    auto windowRef = GameManager::getWindowRef();

    int newHeight = static_cast<int>(windowRef->getMonitorSize().second * 0.75);
    int newWidth = (newHeight * 16) / 9;

    windowRef->setWindowSize(newWidth, newHeight);
    windowRef->centerWindow();
    windowRef->setWindowStyle(Window::Style::UNDECORATED);

    auto onRegisterButtonCallback = []() {
        GameManager::changeScene("RegisterScene");
    };

    auto loginCallback = [this]() {
        if (registry.valid(usernameTextBox) && registry.valid(passwordTextBox)) {
            const std::string& username = registry.get<TextboxComponent>(this->usernameTextBox).text;
            const std::string& password = registry.get<TextboxComponent>(this->passwordTextBox).text;

            try {
                if (ClientNetworkService::login(username, password)) {
                    AT_INFO("Login successfully");
                    GameManager::changeScene("MenuScene");
                }
            } catch (std::runtime_error error) {
                // print msj on ui
                AT_ERROR("Login failed: {0}", error.what());
            }
        }
    };

    this->gameTitle = Actors::createStaticProp(this->registry,
                                               {glm::vec3(-1100.0f, 750.0f, 0.0f), 0.0f, glm::vec2(400.0f, 400.0f)},
                                               {"iconAndLogo", RenderComponent::defaultTexCoords(), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), true}
    );

    this->sideRect = Actors::createStaticProp(this->registry,
                                              {glm::vec3(-1100.0f, 0.0f, 4.0f), 0.0f, glm::vec2(1700.0f, 2400.0f)},
                                              {"", RenderComponent::defaultTexCoords(), Color(22, 22, 22, 200), true}
    );

    // Main background
    this->background = Actors::createStaticProp(this->registry,
                                                {glm::vec3(0.0f, 0.0f, 4.0f), 0.0f, glm::vec2(1000.0f, 500.0f)},
                                                {"background01", RenderComponent::defaultTexCoords(), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), true}
    );

    // Title
    this->title = Actors::createStaticProp(this->registry,
                                           {glm::vec3(-1135.0f, 300.0f, 0.0f), 3.0f, glm::vec2(15.0f, 30.0f)},
                                           {"thaleah", "Sign in", true, Color::white()}
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
                                                  {"", "thaleah", Color::white(), 12, 0, false, false, true}
    );

    // Login button
    this->loginButton = Actors::createButton(this->registry,
                                             {glm::vec3(-1090.0f, -520.0f, 0.0f), 0.0f, glm::vec2(1000.0f, 250.0f)},
                                             {"panel-transparent-border-010", RenderComponent::defaultTexCoords(), Color::white(), true, RENDERER_NINE_SLICE},
                                             {"Hex in", "thaleah",  Color::black(), Color::white(), Color::white(), Color::black(),false, false, Color::white(), Color(109, 52, 133), Color(54, 26, 66),
                                                 Color::gray(),false,loginCallback, nullptr, nullptr}
    );

    // Register button
    this->registerButton = Actors::createButton(this->registry,
                                                {glm::vec3(-1090.0f, -800.0f, 0.0f), 0.0f, glm::vec2(600.0f, 150.0f)},
                                                {"panel-transparent-border-010", RenderComponent::defaultTexCoords(), Color::white(), true, RENDERER_NINE_SLICE},
                                                {"Register", "thaleah",  Color::black(), Color::white(), Color::white(), Color::black(),false, false, Color::white(), Color(109, 52, 133), Color(54, 26, 66),
                                                 Color::gray(),false,onRegisterButtonCallback, nullptr, nullptr}
    );
}

void LoginScene::onUpdate(float deltaTime) {
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
    renderSystem.update(registry);
}

void LoginScene::onRender(int screenWidth, int screenHeight) {
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

void LoginScene::onDestroy() {
    ResourceManager::clearAll();
}
