#pragma once


#include <Atlas.hpp>
#include "system/PawnSystem.hpp"
#include "system/RenderSystem.hpp"
#include "system/UISystem.hpp"
#include "scene/Scene.hpp"

class RegisterScene : public Scene {
public:
    void onCreate() override;

    void onStart() override;

    void onUpdate(float deltaTime) override;

    void onRender(int screenWidth, int screenHeight) override;

    void onDestroy() override;

private:
    Camera camera{{0.0f, 0.0f}, 0.5f};
    Registry registry;

    RenderSystem renderSystem;
    PawnSystem pawnSystem;
    UISystem uiSystem;

    Actor background;
    Actor sideRect;
    Actor gamelogo;
    Actor title;
    Actor usernameTextBox;
    Actor passwordTextBox;
    Actor registerButton;
    Actor backButton;

};
