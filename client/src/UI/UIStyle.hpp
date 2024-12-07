#pragma once
#include "glm/glm.hpp"
#include <string>

struct UIStyle {
    // Colors for different states
    glm::vec4 normalColor{1.0f, 1.0f, 1.0f, 1.0f};
    glm::vec4 hoverColor{0.9f, 0.9f, 0.9f, 1.0f};
    glm::vec4 pressedColor{0.8f, 0.8f, 0.8f, 1.0f};
    glm::vec4 disabledColor{0.5f, 0.5f, 0.5f, 1.0f};
    glm::vec4 textColor{0.0f, 0.0f, 0.0f, 1.0f};

    std::string fontKey{"default"};
    float fontSize{16.0f};
    float padding{5.0f};

    // Setters for colors
    UIStyle& setColor(const glm::vec4& color) {
        normalColor = color;
        hoverColor = color * 0.9f;
        pressedColor = color * 0.8f;
        disabledColor = color * 0.5f;
        return *this;
    }

    UIStyle& setHoverColor(const glm::vec4& color) {
        hoverColor = color;
        return *this;
    }

    UIStyle& setPressedColor(const glm::vec4& color) {
        pressedColor = color;
        return *this;
    }

    UIStyle& setDisabledColor(const glm::vec4& color) {
        disabledColor = color;
        return *this;
    }

    UIStyle& setTextColor(const glm::vec4& color) {
        textColor = color;
        return *this;
    }

    UIStyle& setFont(const std::string& font) {
        fontKey = font;
        return *this;
    }

    UIStyle& setFontSize(float size) {
        fontSize = size;
        return *this;
    }

    UIStyle& setPadding(float pad) {
        padding = pad;
        return *this;
    }

    // Convenience method to set all colors at once
    UIStyle& setColors(const glm::vec4& normal, const glm::vec4& hover,
                      const glm::vec4& pressed, const glm::vec4& disabled) {
        normalColor = normal;
        hoverColor = hover;
        pressedColor = pressed;
        disabledColor = disabled;
        return *this;
    }

    // Convenience method to set text properties
    UIStyle& setTextStyle(const std::string& font, float size, const glm::vec4& color) {
        fontKey = font;
        fontSize = size;
        textColor = color;
        return *this;
    }
};