#pragma once

#include <algorithm>
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

    void setPosition(const glm::vec2 &position, float deltaTime = 0.016f) {
        glm::vec2 targetPos = position;
        if (boundsEnabled) {
            targetPos = calculateBoundedPosition(position);
        }

        if (smoothFollow) {
            // Improved smoothing calculation
            // Use delta time or a more aggressive interpolation
            float interpolationSpeed = 10.0f * smoothness; // More responsive follow
            worldPosition = worldPosition + (targetPos - worldPosition) * interpolationSpeed * deltaTime;
        } else {
            worldPosition = targetPos;
        }
    }

    float getMinimumZoomLevel() const {
        // Calculate minimum zoom required to fit the map in the window
        float minZoomX = windowSize.x / mapSize.x;
        float minZoomY = windowSize.y / mapSize.y;
        if (minZoomX < minZoomY)
            return minZoomY;
        return minZoomX;
    }

    void setZoom(float zoom) {
        // Ensure zoom doesn't go below minimum
        float minZoom = getMinimumZoomLevel();
        if (zoom > getMinimumZoomLevel())
            this->zoomFactor = zoom;
        else
            this->zoomFactor = getMinimumZoomLevel();
    }

    void zoom(float factor) {
        zoomFactor *= factor;
    }

    void position(const glm::vec2 &offset) {
        setPosition(worldPosition + offset);
    }

    void setBounds(const glm::vec2& mapSize) {
        this->mapSize = mapSize;
        boundsEnabled = true;
    }

    void disableBounds() {
        boundsEnabled = false;
    }

    void setSmoothFollow(bool enabled, float smoothness = 0.1f) {
        this->smoothFollow = enabled;
        this->smoothness = glm::clamp(smoothness, 0.0f, 0.5f);
    }

private:
    glm::mat4 projectionMatrix{1.0f}, viewMatrix{1.0f};
    glm::vec2 worldPosition{0.0f, 0.0f};
    glm::vec2 windowSize{};  // Store window size for screen to world conversion
    float zoomFactor;

    // New members for camera control
    glm::vec2 mapSize{5000.0f, 5000.0f}; // Total size from -2450 to 2450
    bool boundsEnabled{false};
    bool smoothFollow{false};
    float smoothness{0.1f};

    glm::vec2 calculateBoundedPosition(const glm::vec2& targetPos) const;
};
