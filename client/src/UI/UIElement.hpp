/*#pragma once
#include "UI/UIComponents.hpp"

class UIElement {
public:
    static Actor create(Registry& registry,
                       const glm::vec3& position,
                       const glm::vec2& size,
                       const UIStyle& style = UIStyle{}) {
        auto entity = registry.create();
        registry.emplace<TransformComponent>(entity, position, 0.0f, size);
        auto& render = registry.emplace<RenderComponent>(entity,
            "default",
            Sprite::defaultTexCoords(),
            style.normalColor);
        auto& ui = registry.emplace<UIComponent>(entity);
        ui.style = style;  // Make sure to store the style
        return entity;
    }

    static void update(Registry& registry, const Camera& camera) {
        updateInteractions(registry, camera);
        updateVisuals(registry);
    }

protected:
    static void updateVisuals(Registry& registry) {
        auto view = registry.view<RenderComponent, UIComponent>();

        for (auto entity : view) {
            auto& render = view.get<RenderComponent>(entity);
            auto& ui = view.get<UIComponent>(entity);

            // Update background color based on state
            switch (ui.state) {
                case UIState::Normal:
                    render.color = ui.style.normalColor;
                    break;
                case UIState::Hovered:
                    render.color = ui.style.hoverColor;
                    break;
                case UIState::Pressed:
                    render.color = ui.style.pressedColor;
                    break;
                case UIState::Disabled:
                    render.color = ui.style.disabledColor;
                    break;
            }

            // Handle visibility
            if (!ui.isVisible) {
                render.color.a = 0.0f;
            }
        }
    }

    static void updateInteractions(Registry& registry, const Camera& camera) {
        auto view = registry.view<TransformComponent, UIComponent>();

        static bool wasPressed = false;  // Track previous mouse state
        bool isMousePressed = Mouse::isButtonPressed(Mouse::ButtonLeft);

        glm::vec2 mousePos = camera.screenToWorld({Mouse::getX(), Mouse::getY()});

        for (auto entity : view) {
            auto& [position, rotation, size] = view.get<TransformComponent>(entity);
            auto& ui = view.get<UIComponent>(entity);

            if (!ui.isEnabled) {
                ui.state = UIState::Disabled;
                continue;
            }

            // Check if mouse is over the button (accounting for centered position)
            bool isInBounds = mousePos.x >= position.x - size.x/2.0f &&
                             mousePos.x <= position.x + size.x/2.0f &&
                             mousePos.y >= position.y - size.y/2.0f &&
                             mousePos.y <= position.y + size.y/2.0f;

            if (isInBounds) {
                if (ui.state == UIState::Normal) {  // Just entered hover state
                    if (ui.onHover) ui.onHover();
                }

                if (isMousePressed) {
                    ui.state = UIState::Pressed;
                } else if (wasPressed && ui.state == UIState::Pressed) {
                    // Mouse was released while over the button - trigger click
                    if (ui.onClick) ui.onClick();
                    ui.state = UIState::Hovered;
                } else {
                    ui.state = UIState::Hovered;
                }
            } else {
                if (ui.state == UIState::Hovered) {  // Just left hover state
                    if (ui.onLeave) ui.onLeave();
                }
                ui.state = UIState::Normal;
            }
        }

        wasPressed = isMousePressed;
    }
};*/