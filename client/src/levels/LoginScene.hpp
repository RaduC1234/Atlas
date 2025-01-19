#pragma once

#include <Atlas.hpp>

#include "scene/Scene.hpp"
#include "system/PawnSystem.hpp"
#include "system/RenderSystem.hpp"
#include "system/UISystem.hpp"

class LoginScene : public Scene {
public:
    void onCreate() override;

    void showErrorMessage(const std::string &message);

    void onStart() override;

    void onUpdate(float deltaTime) override;

    void onRender(int screenWidth, int screenHeight) override;

    void onDestroy() override;

private:
    Camera camera{{0.0f, 0.0f}, 0.5f};

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
    Actor errorMessage;
    float errorMessageTimer = 0.0f;
    const float ERROR_MESSAGE_DURATION = 3.0f;

};
