#pragma once

#include <glm/glm.hpp>
#include <Atlas.hpp>

#include "UI/UIComponent.hpp"
#include "renderer/Camera.hpp"
#include "Button.hpp"

// Assuming Actor = entt::entity and that entt::null is used to represent an invalid Actor
class UISystem {
public:
    void update(Registry& registry, const Camera& camera) {
        // Get mouse position in world coordinates
        auto mousePos = Mouse::getPosition();
        glm::vec2 mousePosition = camera.screenToWorld({mousePos.first, mousePos.second});
        bool isMousePressed = Mouse::isButtonPressed(Mouse::ButtonLeft);
        bool isDragging = Mouse::isDragging();

        // Track previously interacted UI element to handle z-ordering
        Actor hoveredEntity = entt::null; // Use entt::null to represent an invalid actor
        float closestDepth = std::numeric_limits<float>::max();

        // First pass: Find the topmost UI element being hovered
        registry.view<UIComponent, TransformComponent>().each(
            [&](Actor entity, const UIComponent& uiComponent, const TransformComponent& transform) {
                if (isMouseOver(transform, mousePosition)) {
                    if (transform.position.z < closestDepth) {
                        closestDepth = transform.position.z;
                        hoveredEntity = entity;
                    }
                }
            });

        // Second pass: Handle interactions
        registry.view<UIComponent, TransformComponent>().each(
            [&](Actor entity, UIComponent& uiComponent, TransformComponent& transform) {
                handleInteraction(registry, entity, uiComponent, transform, mousePosition,
                                  isMousePressed, isDragging, entity == hoveredEntity);
            });
    }

private:
    void handleInteraction(Registry& registry, Actor entity, UIComponent& uiComponent,
                          TransformComponent& transform, const glm::vec2& mousePosition,
                          bool isMousePressed, bool isDragging, bool isTopmost) {

        bool isHovered = isTopmost && isMouseOver(transform, mousePosition);

        // Update hover state
        if (uiComponent.isHovered != isHovered) {
            uiComponent.isHovered = isHovered;

            // Handle Button specific hover state
            if (uiComponent.type == UIComponent::Type::Button) {
                if (auto* button = dynamic_cast<Button*>(&uiComponent)) {
                    if (isHovered) {
                        button->hover();
                    } else {
                        button->release();  // No argument here, just call release()
                    }
                }
            }
        }

        // Handle button interactions
        if (uiComponent.type == UIComponent::Type::Button) {
            auto* button = dynamic_cast<Button*>(&uiComponent);
            if (button) {
                if (isHovered && isMousePressed && !uiComponent.isPressed) {
                    uiComponent.isPressed = true;
                    button->press();
                }
                else if (uiComponent.isPressed && !isMousePressed) {
                    uiComponent.isPressed = false;
                    if (isHovered) {  // Only trigger if still hovering
                        button->release();  // No argument here, just call release()
                    }
                }
            }
        }

        // Handle dragging state
        if (isDragging && isHovered && !uiComponent.isPressed) {
            uiComponent.isPressed = true;
        }
        else if (!isDragging && uiComponent.isPressed) {
            uiComponent.isPressed = false;
        }
    }

    bool isMouseOver(const TransformComponent& transform, const glm::vec2& mousePosition) const {
        // Calculate UI element bounds considering rotation
        glm::vec2 position(transform.position.x, transform.position.y);
        glm::vec2 halfSize = transform.scale * 0.5f;

        // Transform mouse position relative to element center
        glm::vec2 localMousePos = mousePosition - (position + halfSize);

        // Apply inverse rotation
        float cosA = cos(-transform.rotation);
        float sinA = sin(-transform.rotation);
        glm::vec2 rotatedMousePos(
            localMousePos.x * cosA - localMousePos.y * sinA,
            localMousePos.x * sinA + localMousePos.y * cosA
        );

        // Check if point is within bounds
        return (abs(rotatedMousePos.x) <= halfSize.x &&
                abs(rotatedMousePos.y) <= halfSize.y);
    }
};
