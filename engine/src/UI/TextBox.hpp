//
// Created by Karina on 11/10/2024.
//

#pragma once

#include <entt/entt.hpp>
#include <string>
#include <glm/vec2.hpp>

struct Position {
    float x, y;
};

struct Size {
    float width, height;
};

struct Text {
    std::string content;
    float fontSize;
};

struct TextBoxComponent {
    bool isEditable;
};

class TextBox {
public:
    static void initialize(entt::registry& registry, entt::entity entity, float x, float y, float width, float height, const std::string& initialText, bool isEditable) {
        registry.emplace<Position>(entity, x, y);
        registry.emplace<Size>(entity, width, height);
        registry.emplace<Text>(entity, initialText, 16.0f);
        registry.emplace<TextBoxComponent>(entity, isEditable);
    }

    static void render(const entt::registry& registry, entt::entity entity) {
        const auto& pos = registry.get<Position>(entity);
        const auto& size = registry.get<Size>(entity);
        const auto& text = registry.get<Text>(entity);

        renderRectangle(pos.x, pos.y, size.width, size.height);
        renderText(text.content, pos.x + 0.0f, pos.y + 0.0f, text.fontSize);
    }

    static void handleInput(entt::registry& registry, entt::entity entity, const glm::vec2& mousePosition, bool isClicked, char inputChar) {
        auto& pos = registry.get<Position>(entity);
        auto& size = registry.get<Size>(entity);
        auto& text = registry.get<Text>(entity);
        auto& textBox = registry.get<TextBoxComponent>(entity);

        bool isWithinBounds =
            mousePosition.x >= pos.x && mousePosition.x <= pos.x + size.width &&
            mousePosition.y >= pos.y && mousePosition.y <= pos.y + size.height;

        if (isWithinBounds && isClicked && textBox.isEditable) {
            if (inputChar) {
                text.content += inputChar;
            }
        }
    }
};

