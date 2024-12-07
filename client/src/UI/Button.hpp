#pragma once
#include "UIElement.hpp"
#include "UI/UIStyle.hpp"

struct ButtonSprite {
    std::string textureKey;  // Texture resource ID
    TextureCoords coords;    // Texture coordinates for different states
    bool useSprite{false};   // Whether to use sprite or solid color

    ButtonSprite() = default;

    ButtonSprite(const std::string& texture, const TextureCoords& texCoords = Sprite::defaultTexCoords())
        : textureKey(texture), coords(texCoords), useSprite(true) {}
};

class Button : public UIElement {
public:

    static Actor create(Registry& registry,
                       const glm::vec3& position,
                       const glm::vec2& size,
                       const std::string& text,
                       std::function<void()> onClick,
                       const UIStyle& style = UIStyle{},
                       const ButtonSprite& sprite = ButtonSprite()) {

        // Create base UI element with style
        auto buttonEntity = UIElement::create(registry, position, size, style);

        // Configure render component for sprite if provided
        auto& render = registry.get<RenderComponent>(buttonEntity);
        if (sprite.useSprite) {
            render.textureKey = sprite.textureKey;
            render.coords = sprite.coords;
        }

        // Add UI component and configure it
        auto& ui = registry.get<UIComponent>(buttonEntity);
        ui.text = text;
        ui.size = size;
        ui.position = glm::vec2(position);
        ui.style = style;
        ui.onClick = std::move(onClick);
        ui.isEnabled = true;
        ui.isVisible = true;

        // Add button component
        auto& button = registry.emplace<ButtonComponent>(buttonEntity);
        button.onClick = ui.onClick;
        button.textEntity = createTextElement(registry, buttonEntity, text, style);

        return buttonEntity;
    }

private:
    static Actor createTextElement(Registry& registry,
                                 Actor buttonEntity,
                                 const std::string& text,
                                 const UIStyle& style) {
        if (text.empty()) return entt::null;

        auto& buttonTransform = registry.get<TransformComponent>(buttonEntity);

        auto textEntity = registry.create();

        // Calculate text position and scale
        float textScale = style.fontSize * (buttonTransform.scale.y / 100.0f);

        // Position text centered on button
        glm::vec3 textPos = buttonTransform.position;
        textPos.z = buttonTransform.position.z - 0.1f;

        // Create text transform
        registry.emplace<TransformComponent>(textEntity,
            textPos,
            0.0f,
            glm::vec2(textScale)
        );

        // Create text render component
        auto& textRender = registry.emplace<RenderComponent>(textEntity,
            style.fontKey,
            text,
            style.textColor
        );
        textRender.shape = Shape::TEXT;

        // Add UI component for text styling
        auto& textUI = registry.emplace<UIComponent>(textEntity);
        textUI.style = style;
        textUI.text = text;
        textUI.isEnabled = false;
        textUI.isVisible = true;

        return textEntity;
    }

    static void updateButtons(Registry& registry) {
        auto view = registry.view<ButtonComponent, UIComponent, TransformComponent, RenderComponent>();

        for (auto entity : view) {
            auto& button = view.get<ButtonComponent>(entity);
            auto& ui = view.get<UIComponent>(entity);
            auto& transform = view.get<TransformComponent>(entity);
            auto& render = view.get<RenderComponent>(entity);

            // Update color/alpha based on state while preserving texture
            glm::vec4 stateColor;
            switch (ui.state) {
                case UIState::Normal:
                    stateColor = ui.style.normalColor;
                    break;
                case UIState::Hovered:
                    stateColor = ui.style.hoverColor;
                    break;
                case UIState::Pressed:
                    stateColor = ui.style.pressedColor;
                    break;
                case UIState::Disabled:
                    stateColor = ui.style.disabledColor;
                    break;
            }

            // Blend the state color with existing color
            render.color = stateColor * glm::vec4(1.0f, 1.0f, 1.0f, render.color.a);

            // Update text if it exists
            if (button.textEntity != entt::null && registry.valid(button.textEntity)) {
                auto& textTransform = registry.get<TransformComponent>(button.textEntity);
                auto& textRender = registry.get<RenderComponent>(button.textEntity);

                // Calculate text dimensions
                float charWidth = ui.style.fontSize * 0.5f;
                float textWidth = ui.text.length() * charWidth;
                float textHeight = ui.style.fontSize;

                // Center text within button
                textTransform.position.x = transform.position.x - (textWidth * 0.5f);
                textTransform.position.y = transform.position.y - (textHeight * 0.25f);
                textTransform.position.z = transform.position.z - 0.1f;

                // Update text color based on button state
                textRender.color = ui.state == UIState::Disabled ?
                    ui.style.disabledColor : ui.style.textColor;

                // Handle text visibility
                textRender.color.a = ui.isVisible ? textRender.color.a : 0.0f;
            }

            // Store button state
            button.isHovered = ui.state == UIState::Hovered;
            button.isPressed = ui.state == UIState::Pressed;
        }
    }

public:
    static void update(Registry& registry, const Camera& camera) {
        UIElement::update(registry, camera);
        updateButtons(registry);
    }

    static void setSprite(Registry& registry, Actor buttonEntity, const ButtonSprite& sprite) {
        if (auto* render = registry.try_get<RenderComponent>(buttonEntity)) {
            render->textureKey = sprite.textureKey;
            render->coords = sprite.coords;
        }
    }

    // Configuration methods
    static void setText(Registry& registry, Actor buttonEntity, const std::string& newText) {
        if (auto* button = registry.try_get<ButtonComponent>(buttonEntity)) {
            if (auto* ui = registry.try_get<UIComponent>(buttonEntity)) {
                ui->text = newText;
                if (button->textEntity != entt::null && registry.valid(button->textEntity)) {
                    if (auto* textRender = registry.try_get<RenderComponent>(button->textEntity)) {
                        textRender->text = newText;
                    }
                    if (auto* textUI = registry.try_get<UIComponent>(button->textEntity)) {
                        textUI->text = newText;
                    }
                }
            }
        }
    }

    static void setEnabled(Registry& registry, Actor buttonEntity, bool enabled) {
        if (auto* ui = registry.try_get<UIComponent>(buttonEntity)) {
            ui->isEnabled = enabled;
            ui->state = enabled ? UIState::Normal : UIState::Disabled;
        }
    }

    static void setVisible(Registry& registry, Actor buttonEntity, bool visible) {
        if (auto* ui = registry.try_get<UIComponent>(buttonEntity)) {
            ui->isVisible = visible;
            if (auto* button = registry.try_get<ButtonComponent>(buttonEntity)) {
                if (button->textEntity != entt::null && registry.valid(button->textEntity)) {
                    if (auto* textUI = registry.try_get<UIComponent>(button->textEntity)) {
                        textUI->isVisible = visible;
                    }
                }
            }
        }
    }

    static void setStyle(Registry& registry, Actor buttonEntity, const UIStyle& style) {
        if (auto* ui = registry.try_get<UIComponent>(buttonEntity)) {
            ui->style = style;
            if (auto* button = registry.try_get<ButtonComponent>(buttonEntity)) {
                if (button->textEntity != entt::null && registry.valid(button->textEntity)) {
                    if (auto* textUI = registry.try_get<UIComponent>(button->textEntity)) {
                        textUI->style = style;
                    }
                }
            }
        }
    }

    static void setOnClick(Registry& registry, Actor buttonEntity, std::function<void()> onClick) {
        if (auto* ui = registry.try_get<UIComponent>(buttonEntity)) {
            ui->onClick = onClick;
            if (auto* button = registry.try_get<ButtonComponent>(buttonEntity)) {
                button->onClick = onClick;
            }
        }
    }

    static void setPosition(Registry& registry, Actor buttonEntity, const glm::vec3& position) {
        if (auto* transform = registry.try_get<TransformComponent>(buttonEntity)) {
            transform->position = position;
            if (auto* ui = registry.try_get<UIComponent>(buttonEntity)) {
                ui->position = glm::vec2(position);
            }
        }
    }

    static void setSize(Registry& registry, Actor buttonEntity, const glm::vec2& size) {
        if (auto* transform = registry.try_get<TransformComponent>(buttonEntity)) {
            transform->scale = size;
            if (auto* ui = registry.try_get<UIComponent>(buttonEntity)) {
                ui->size = size;
            }
        }
    }
};