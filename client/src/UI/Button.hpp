#pragma once

#include <glm/vec2.hpp>
#include <string>
#include <functional>
#include "UIComponent.hpp"
#include "renderer/Color.hpp"
#include "renderer/Sprite.hpp"
#include "renderer/Texture.hpp"

class Button : public UIComponent {
public:
    struct ButtonStyle {
        // Visual state colors for when no texture is used
        Color NormalColor{1.0f, 1.0f, 1.0f, 1.0f};
        Color HoverColor{0.9f, 0.9f, 0.9f, 1.0f};
        Color PressedColor{0.8f, 0.8f, 0.8f, 1.0f};

        // Text properties
        std::string Text;
        std::string FontKey;
        Color TextColor{0.0f, 0.0f, 0.0f, 1.0f};
    };

    // Constructor for basic colored/text button
    Button(const glm::vec2& position, const glm::vec2& size, const ButtonStyle& style)
        : position(position)
        , size(size)
        , style(style)
        , currentColor(style.NormalColor)
        , sprite(nullptr)  // No sprite
    {
        type = Type::Button;
    }

    // Constructor for sprite-based button
    Button(const glm::vec2& position, const glm::vec2& size, Ref<Texture> texture, const std::string& text = "")
        : position(position)
        , size(size)
        , sprite(CreateRef<Sprite>(texture))
        , currentColor(Color(1.0f, 1.0f, 1.0f, 1.0f))
    {
        type = Type::Button;
        style.Text = text;
    }

    // State handlers
    void hover() {
        if (sprite) {
            currentColor = Color(0.9f, 0.9f, 0.9f, 1.0f);  // Slightly dim the sprite
        } else {
            currentColor = style.HoverColor;
        }
    }

    void press() {
        if (sprite) {
            currentColor = Color(0.8f, 0.8f, 0.8f, 1.0f);  // Dim the sprite more
        } else {
            currentColor = style.PressedColor;
        }
    }

    void release(Actor actor) {
        currentColor = Color(1.0f, 1.0f, 1.0f, 1.0f);
        if (onClick) onClick();
    }

    // Get render component for the renderer
    RenderComponent getRenderComponent() const {
        if (sprite && sprite->texture) {
            return RenderComponent(
                sprite->texture->getFilePath(),
                sprite->texCoords,
                currentColor
            );
        }

        if (!style.Text.empty()) {
            return RenderComponent(style.Text, style.FontKey);
        }

        // Fallback to colored quad
        return RenderComponent("", Sprite::defaultTexCoords(), currentColor);
    }

    // Getters
    const glm::vec2& getPosition() const { return position; }
    const glm::vec2& getSize() const { return size; }
    const std::string& getText() const { return style.Text; }
    bool hasSprite() const { return sprite != nullptr && sprite->texture != nullptr; }

    void setSprite(Ref<Texture> texture) {
        sprite = CreateRef<Sprite>(texture);
    }

private:
    glm::vec2 position;
    glm::vec2 size;
    ButtonStyle style;
    Color currentColor;
    Ref<Sprite> sprite;
};