#pragma once

#include <Atlas.hpp>
#include <glm/glm.hpp>

#include "renderer/Camera.hpp"
#include "window/Keyboard.hpp"
#include "window/Mouse.hpp"

class PawnSystem {
public:

    void update(double deltaTime, Registry &registry, uint32_t playerId, Camera &camera) {
        // Retrieve the mouse position in screen coordinates
        auto x = Mouse::getPosition(); // Assuming Mouse::getPosition() exists
        glm::vec2 mouseWorldPos = camera.screenToWorld({x.first, x.second}); // Convert to world position

        // === Step 2: Update Textures for All Pawns ===
        registry.view<PawnComponent, RenderComponent>().each([&](auto entity, PawnComponent &pawn, RenderComponent &render) {
            if (pawn.moveForward) {
                render.textureKey = "back1";
            } else if (pawn.moveBackwards) {
                render.textureKey = "front1";
            } else if (pawn.moveLeft) {
                render.textureKey = "left1";
            } else if (pawn.moveRight) {
                render.textureKey = "right1";
            }
        });
    }
};
