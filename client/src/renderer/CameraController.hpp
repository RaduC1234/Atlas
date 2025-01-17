#pragma once

#include <glm/glm.hpp>
#include "Camera.hpp"
#include <entt/entt.hpp>
#include "entity/Components.hpp"

class CameraController {
public:
    CameraController(Camera& camera) : camera(camera) {
        // Initialize with some default values
        camera.setSmoothFollow(0.1f);  // Smooth follow
        camera.setZoom(1.0f);        // Default zoom level
    }

    void update(entt::registry& registry, uint64_t targetPlayerId, float deltaTime) {
        // Find the player entity with the matching ID
        auto view = registry.view<const PawnComponent, const TransformComponent>();

        for (auto [entity, pawn, transform] : view.each()) {
            if (pawn.playerId == targetPlayerId) {
                // Update camera position to follow player
                glm::vec2 targetPos(transform.position.x, transform.position.y);
                camera.setPosition(targetPos, deltaTime);
                break;
            }
        }
    }

    // Camera control methods
    void setZoom(float zoom) {
        camera.setZoom(zoom);
    }

    void setSmoothness(float smoothness) {
        camera.setSmoothFollow(true,smoothness);
    }

    void setBounds(const glm::vec2& mapSize) {
        camera.setBounds(mapSize);
    }

private:
    Camera& camera;
};