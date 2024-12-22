//
// Created by Karina on 12/21/2024.
//
#pragma once

#include <imgui.h>
#include <Atlas.hpp>

#include "system/PawnSystem.hpp"
#include "system/RenderSystem.hpp"
#include "system/UISystem.hpp"
#include "network/ClientNetworkService.hpp"
#include "renderer/Color.hpp"
#include "scene/Scene.hpp"

class GameModeScene : public Scene {
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
    Actor title;
    Actor hexDuelButton;
    Actor hexDuelDescription;
    Actor hexArenaDescription;
    Actor hexArenaButton;
    Actor backButton;
};

