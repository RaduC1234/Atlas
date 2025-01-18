#pragma once

#include <Atlas.hpp>

#include "scene/Scene.hpp"
#include "system/PawnSystem.hpp"
#include "system/RenderSystem.hpp"
#include "system/UISystem.hpp"

class GameModeScene : public Scene {
public:
    void onCreate() override;

    void disableButtons();

    void enableButtons();

    void disableLeaveQueueButton();

    void enableLeaveQueueButton(std::function<void()> onClick);

    void onStart() override;

    void onUpdate(float deltaTime) override;

    void onRender(int screenWidth, int screenHeight) override;

    void onDestroy() override;

private:
    Camera camera{{0.0f, 0.0f}, 0.5f};

    bool hexDuelpressed = false;
    bool hexArenapressed = false;
    bool hexDuelDisabled = false;
    bool hexArenaDisabled = false;

    RenderSystem renderSystem;
    PawnSystem pawnSystem;
    UISystem uiSystem;

    Actor background;
    Actor sideRect;
    Actor title;
    Actor hexDuelButton;
    Actor hexDuelDescription;
    Actor hexArenaDescription;
    Actor hexArenaButton;
    Actor backButton;
    Actor findDuelButton;
    Actor findArenaButton;
    Actor stopFindingButton;
    Actor leaveQueueButton;
};
