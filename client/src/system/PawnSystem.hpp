#pragma once

#include <Atlas.hpp>
#include <glm/glm.hpp>


class PawnSystem {
public:
    PawnSystem(float moveSpeed = 200.0f)
        : moveSpeed(moveSpeed) {
    }

    void update(double deltaTime, Registry &registry, uint32_t playerId) {

        registry.view<TransformComponent, PawnComponent, RenderComponent>().each([&](auto entity, TransformComponent &transform, PawnComponent &pawn, RenderComponent &render) {

            if (!playerId == pawn.playerId)
                return;

            glm::vec3 direction(0.0f);

            // Check for keyboard inputs
            if (Keyboard::isKeyPressed(Keyboard::W)) {
                direction.y += 1.0f; // Move up
                render.textureKey = "back1";
            }
            if (Keyboard::isKeyPressed(Keyboard::S)) {
                direction.y -= 1.0f; // Move down
                render.textureKey = "front1";
            }
            if (Keyboard::isKeyPressed(Keyboard::A)) {
                direction.x -= 1.0f; // Move left
                render.textureKey = "left1";
            }
            if (Keyboard::isKeyPressed(Keyboard::D)) {
                direction.x += 1.0f; // Move right
                render.textureKey = "right1";
            }

            if (glm::length(direction) > 0.0f) {
                direction = glm::normalize(direction);
            }

            transform.position += direction * static_cast<float>(deltaTime) * moveSpeed;
        });
    }

private:
    float moveSpeed;
};
