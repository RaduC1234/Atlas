#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Camera {
public:
    Camera() = default;

    Camera(const glm::vec2 &worldPosition, float zoom = 1.0f);

    void applyViewport(int windowWidth, int windowHeight, float desiredAspectRatio = 16.0f / 9.0f);

    glm::vec2 screenToWorld(const glm::vec2 &screenCoords) const;

    glm::vec2 worldToScreen(const glm::vec2 &worldCoords) const;

    glm::mat4 getViewMatrix();

    glm::mat4 getProjectionMatrix() const {
        return projectionMatrix;
    }

    glm::vec2 getPosition() const {
        return worldPosition;
    }

    void setPosition(const glm::vec2 &position) {
        worldPosition = position;
    }

    void zoom(float factor) {
        zoomFactor *= factor;
    }

    void position(const glm::vec2 &offset) {
        worldPosition += offset;
    }

private:
    glm::mat4 projectionMatrix{1.0f}, viewMatrix{1.0f};
    glm::vec2 worldPosition{0.0f, 0.0f};
    glm::vec2 windowSize{};  // Store window size for screen to world conversion
    float zoomFactor;
};
