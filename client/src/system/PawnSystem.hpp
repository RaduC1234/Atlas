#pragma once

#include <glm/glm.hpp>
#include "component/Components.hpp"
#include "scene/Entity.hpp"
#include "renderer/Camera.hpp"

class PawnSystem {
public:
    PawnSystem(float moveSpeed = 200.0f, float cameraSmoothing = 0.1f)
        : moveSpeed(moveSpeed), cameraSmoothing(cameraSmoothing) {}

    void update(double deltaTime, Registry& registry, Camera& camera) {
        auto view = registry.view<TransformComponent, PawnComponent, AnimationComponent>();

        for (const auto& entity : view) {
            auto& transform = view.get<TransformComponent>(entity);
            auto& animation = view.get<AnimationComponent>(entity);

            glm::vec2 direction(0.0f); // Direction is in 2D space
            if (Keyboard::isKeyPressed(Keyboard::A)) {
                direction.x = -1.0f;
            }
            if (Keyboard::isKeyPressed(Keyboard::D)) {
                direction.x = 1.0f;
            }
            if (Keyboard::isKeyPressed(Keyboard::W)) {
                direction.y = 1.0f;
            }
            if (Keyboard::isKeyPressed(Keyboard::S)) {
                direction.y = -1.0f;
            }

            // Handle movement
            if (direction != glm::vec2(0.0f)) {
                direction = glm::normalize(direction);
                transform.position += glm::vec3(
                    direction.x * static_cast<float>(deltaTime) * moveSpeed,
                    direction.y * static_cast<float>(deltaTime) * moveSpeed,
                    0.0f
                ); // Update position in 3D space

                // Update animation
                if (std::abs(direction.x) > std::abs(direction.y)) {
                    animation.setAnimation(direction.x > 0 ? "walk_right" : "walk_left");
                } else {
                    animation.setAnimation(direction.y > 0 ? "walk_up" : "walk_down");
                }
            } else {
                animation.pause(); // Pause animation if no movement
            }

            // Smooth camera movement
            glm::vec2 cameraTarget = glm::vec2(transform.position.x, transform.position.y); // Use only 2D components
            glm::vec2 smoothedPosition = glm::mix(camera.getPosition(), cameraTarget, cameraSmoothing);
            camera.setPosition(smoothedPosition); // Update the camera position
        }
    }

private:
    float moveSpeed;          // Speed of character movement
    float cameraSmoothing;    // Smoothing factor for camera transitions
};
