#pragma once

#include <Atlas.hpp>
#include <glm/glm.hpp>

#include "window/Keyboard.hpp"
#include "window/Mouse.hpp"

class PawnSystem {
public:
    explicit PawnSystem(float moveSpeed = 200.0f)
        : moveSpeed(moveSpeed) {
    }

    void update(double deltaTime, Registry &registry, uint32_t playerId, Camera &camera) {
        // Retrieve the mouse position in screen coordinates
        auto x = Mouse::getPosition(); // Assuming Mouse::getPosition() exists
        glm::vec2 mouseWorldPos = camera.screenToWorld({x.first, x.second}); // Convert to world position

        registry.view<TransformComponent, PawnComponent, RenderComponent>().each([&](auto entity, TransformComponent &transform, PawnComponent &pawn, RenderComponent &render) {

            if (playerId != pawn.playerId)
                return;

            glm::vec3 direction(0.0f);

            // Reset movement flags
            pawn.moveForward = false;
            pawn.moveBackwards = false;
            pawn.moveLeft = false;
            pawn.moveRight = false;

            // Check for keyboard inputs and set movement flags
            if (Keyboard::isKeyPressed(Keyboard::W)) {
                pawn.moveForward = true;
                render.textureKey = "back1";
            }
            if (Keyboard::isKeyPressed(Keyboard::S)) {
                pawn.moveBackwards = true;
                render.textureKey = "front1";
            }
            if (Keyboard::isKeyPressed(Keyboard::A)) {
                pawn.moveLeft = true;
                render.textureKey = "left1";
            }
            if (Keyboard::isKeyPressed(Keyboard::D)) {
                pawn.moveRight = true;
                render.textureKey = "right1";
            }


            // Optionally, calculate and store rotation towards the mouse position
            glm::vec2 toMouse = mouseWorldPos - glm::vec2(transform.position.x, transform.position.y);
            float angle = glm::atan(toMouse.y, toMouse.x);
            pawn.aimRotation = angle;

            // Debugging rotation (can remove this in production)
            ImGui::Text(std::to_string(glm::degrees(angle)).c_str());
        });
    }

private:
    float moveSpeed;
};
