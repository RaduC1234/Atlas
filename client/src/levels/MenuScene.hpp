#pragma once

#include <imgui.h>
#include <Atlas.hpp>
#include "scene/Scene.hpp"
#include "system/PawnSystem.hpp"
#include "system/RenderSystem.hpp"
#include "system/UISystem.hpp"

class MenuScene : public Scene {
public:
    void onCreate() override;

    void onStart() override;

    void onUpdate(float deltaTime) override;

    void onRender(int screenWidth, int screenHeight) override;

    void onDestroy() override;

private:
    Camera camera{{0.0f, 0.0f}, 0.5f};

    RenderSystem renderSystem;
    PawnSystem pawnSystem;
    UISystem uiSystem;

    Actor background;
    Actor sideRect;
    Actor gameTitle;
    Actor playButton;
    Actor optionsButton;
    Actor quitButton;
};
