#pragma once
#include <functional>
#include <string>
#include <glm/glm.hpp>
#include "scene/Entity.hpp"
#include "core/Mouse.hpp"

struct ButtonComponent {
    std::function<void()> onClick;
    bool isHovered{false};
    bool isPressed{false};
    Actor textEntity{entt::null};
    float textWidth;  // Store the width of the text for dynamic centering

    ButtonComponent() = default;
    explicit ButtonComponent(std::function<void()> clickHandler, Actor textEnt = entt::null, float tWidth = 0.0f)
        : onClick(std::move(clickHandler)), textEntity(textEnt), textWidth(tWidth) {}
};

class Button {
public:
    // Helper function to calculate approximate text width based on font metrics
    static float calculateTextWidth(const std::string& text, float fontSize) {
        // These values should be adjusted based on your font metrics
        const float CHAR_WIDTH_RATIO = 0.6f;  // Width of character relative to font size
        const float SPACE_WIDTH_RATIO = 0.3f;  // Width of space relative to font size

        float totalWidth = 0.0f;
        for (char c : text) {
            if (c == ' ') {
                totalWidth += fontSize * SPACE_WIDTH_RATIO;
            } else {
                totalWidth += fontSize * CHAR_WIDTH_RATIO;
            }
        }
        return totalWidth;
    }

    static Actor create(Registry& registry,
                       const glm::vec3& position,
                       const glm::vec2& size,
                       const std::string& textureKey,
                       const glm::vec4& color,
                       const std::string& text,
                       std::function<void()> onClick) {

        // Create the button background
        auto buttonEntity = registry.create();
        registry.emplace<TransformComponent>(buttonEntity, position, 0.0f, size);
        registry.emplace<RenderComponent>(buttonEntity, textureKey, Sprite::defaultTexCoords(), color);

        // Create text entity
        auto textEntity = registry.create();

        // Calculate initial text scale based on button height
        float textScale = size.y * 0.6f;  // Initial scale relative to button height
        float textWidth = calculateTextWidth(text, textScale);

        registry.emplace<TransformComponent>(textEntity,
            position,
            0.0f,
            glm::vec2{textScale});

        // Create text render component
        auto textRender = RenderComponent("minecraft", text, {1.0f, 1.0f, 1.0f, 1.0f});
        textRender.shape = Shape::TEXT;
        textRender.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
        registry.emplace<RenderComponent>(textEntity, textRender);

        // Store text width for dynamic centering
        registry.emplace<ButtonComponent>(buttonEntity, std::move(onClick), textEntity, textWidth);

        return buttonEntity;
    }

    static void updateVisuals(Registry& registry) {
        auto view = registry.view<TransformComponent, RenderComponent, ButtonComponent>();

        for (auto entity : view) {
            auto& render = view.get<RenderComponent>(entity);
            auto& button = view.get<ButtonComponent>(entity);

            if (button.textEntity != entt::null && registry.valid(button.textEntity)) {
                auto& buttonTransform = registry.get<TransformComponent>(entity);
                auto& textTransform = registry.get<TransformComponent>(button.textEntity);
                auto& textRender = registry.get<RenderComponent>(button.textEntity);

                // Calculate text scale based on button size
                float baseTextScale = buttonTransform.scale.y * 0.05f;
                float textWidth = calculateTextWidth(textRender.text, baseTextScale);

                // Adjust scale if text is too wide for button
                float maxWidth = buttonTransform.scale.x * 0.9f; // Leave some padding
                if (textWidth > maxWidth) {
                    baseTextScale *= maxWidth / textWidth;
                    textWidth = maxWidth;
                }

                // Calculate centering offset based on actual text width
                float xOffset = (buttonTransform.scale.x - textWidth) / 2.0f;

                // Update text position and scale
                textTransform.position = {
                    buttonTransform.position.x + (textWidth / 2.0f) - 65.0f,  // Center considering actual text width
                    buttonTransform.position.y - 53.0f,  // Vertical center
                    buttonTransform.position.z - 0.1f  // Slightly in front
                };
                textTransform.scale = glm::vec2(baseTextScale);
            }

            // Update button visuals based on state
            if (button.isPressed) {
                render.color.a = 0.5f;
            } else if (button.isHovered) {
                render.color.a = 0.8f;
            } else {
                render.color.a = 1.0f;
            }
        }
    }

    static void update(Registry& registry, const Camera& camera) {
        auto view = registry.view<TransformComponent, RenderComponent, ButtonComponent>();

        glm::vec2 mousePos = camera.screenToWorld({Mouse::getX(), Mouse::getY()});

        for (auto entity : view) {
            auto& [position, rotation, size] = view.get<TransformComponent>(entity);
            auto& button = view.get<ButtonComponent>(entity);

            float halfWidth = size.x / 2.0f;
            float halfHeight = size.y / 2.0f;

            bool isInBounds = mousePos.x >= position.x - halfWidth &&
                             mousePos.x <= position.x + halfWidth &&
                             mousePos.y >= position.y - halfHeight &&
                             mousePos.y <= position.y + halfHeight;

            button.isHovered = isInBounds;

            if (isInBounds) {
                if (Mouse::isButtonPressed(Mouse::ButtonLeft)) {
                    if (!button.isPressed) {
                        button.isPressed = true;
                    }
                } else if (button.isPressed) {
                    button.isPressed = false;
                    if (button.onClick) {
                        button.onClick();
                    }
                }
            } else {
                button.isPressed = false;
            }
        }
    }

};