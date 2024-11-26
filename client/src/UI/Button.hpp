#pragma once

#include <glm/vec2.hpp>
#include <string>
#include <functional>
#include "UIComponent.hpp"
#include "renderer/Color.hpp"
#include "renderer/Sprite.hpp"
#include "renderer/Texture.hpp"
#include "renderer/RenderManager.hpp"

class Button : public UIComponent {
public:
    struct ButtonStyle {
        Color NormalColor{1.0f, 1.0f, 1.0f, 1.0f};
        Color HoverColor{0.9f, 0.9f, 0.9f, 1.0f};
        Color PressedColor{0.8f, 0.8f, 0.8f, 1.0f};
        std::string Text;
        Ref<Font> FontKey;
        Color TextColor{0.0f, 0.0f, 0.0f, 1.0f};
    };

    // Constructor for basic colored/text button
    Button(const glm::vec2& position, const glm::vec2& size, const ButtonStyle& style)
        : position(position)
        , size(size)
        , style(style)
        , currentColor(style.NormalColor)
        , sprite(nullptr) // No sprite
    {
        type = UIComponent::Type::Button;
    }

    // Constructor for sprite-based button
    Button(const glm::vec2& position, const glm::vec2& size, const Ref<Texture> texture, const std::string& text = "")
        : position(position)
        , size(size)
        , sprite(CreateRef<Sprite>(texture))
        , currentColor(Color(1.0f, 1.0f, 1.0f, 1.0f))
    {
        type = UIComponent::Type::Button;
        style.Text = text;
    }

    // Interaction handlers
    void hover() {
        currentColor = sprite ? Color(0.9f, 0.9f, 0.9f, 1.0f) : style.HoverColor;
    }

    void press() {
        currentColor = sprite ? Color(0.8f, 0.8f, 0.8f, 1.0f) : style.PressedColor;
    }

    void release() {
        currentColor = style.NormalColor;
        if (onClick) onClick();
    }

    // Rendering the button
    void render() const {
        // Draw the button background
        if (sprite && sprite->texture) {
            RenderManager::drawQuad(glm::vec3(position, 0.0f), size, currentColor, *sprite);
        } else {
            RenderManager::drawQuad(glm::vec3(position, 0.0f), size, currentColor);
        }

        // Draw the button text
        if (!style.Text.empty()) {
            glm::vec3 textPosition(position.x + size.x / 2, position.y + size.y / 2, 1.0f); // Center text in button
            RenderManager::drawText(
                textPosition,
                style.Text,
                style.FontKey,
                size.y * 0.4f,                      // Scale text size relative to button height
                style.TextColor,
                true                                // Center-align text
            );
        }
    }

    // Customization methods
    void setStyle(const ButtonStyle& newStyle) { style = newStyle; }
    void setNormalColor(const Color& color) { style.NormalColor = color; }
    void setHoverColor(const Color& color) { style.HoverColor = color; }
    void setPressedColor(const Color& color) { style.PressedColor = color; }
    void setText(const std::string& text) { style.Text = text; }
    void setTextColor(const Color& color) { style.TextColor = color; }
    void setFont(const Ref<Font>& font) { style.FontKey = font; }
    void setOnClick(const std::function<void()>& callback) { onClick = callback; }

    // Getters
    const glm::vec2& getPosition() const { return position; }
    const glm::vec2& getSize() const { return size; }
    const std::string& getText() const { return style.Text; }
    const Color& getNormalColor() const { return style.NormalColor; }
    const Color& getHoverColor() const { return style.HoverColor; }
    const Color& getPressedColor() const { return style.PressedColor; }
    const Color& getTextColor() const { return style.TextColor; }
    bool hasSprite() const { return sprite != nullptr && sprite->texture != nullptr; }

    void setPosition(const glm::vec2& newPosition) { position = newPosition; }
    void setSize(const glm::vec2& newSize) { size = newSize; }
    void setSprite(const Ref<Texture>& texture) {
        sprite = CreateRef<Sprite>(texture);
    }

private:
    glm::vec2 position;
    glm::vec2 size;
    ButtonStyle style;
    Color currentColor;
    Ref<Sprite> sprite;
    std::function<void()> onClick;
};
