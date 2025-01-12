#pragma once

#include "Atlas.hpp"
#include "renderer/Camera.hpp"
#include "scene/Scene.hpp"
#include "system/NetworkSystem.hpp"
#include "system/PawnSystem.hpp"
#include "system/RenderSystem.hpp"

class MatchScene : public Scene {
public:
    void onCreate() override;

    void onStart() override;

    void onUpdate(float deltaTime) override;

    void onRender(int screenWidth, int screenHeight) override;

    void onDestroy() override;

private:
    Camera camera{{0.0f, 0.0f}, 1.0f};
    uint64_t playerId;
    Registry registry;

    PawnSystem pawnSystem;
    RenderSystem renderSystem;
    NetworkSystem networkSystem;
};
