/*#pragma once
#include "UIStyle.hpp"
#include "UIForward.hpp"
#include <string>
#include <algorithm>
#include <utility>
#include <functional>
#include "entity/Entity.hpp"

struct UIComponent {
    UIStyle style;
    UIState state{UIState::Normal};
    bool isVisible{true};
    bool isEnabled{true};
    bool isFocused{false};

    glm::vec2 size{100.0f, 30.0f};
    glm::vec2 position{0.0f, 0.0f};
    std::string text;

    std::function<void()> onHover;
    std::function<void()> onLeave;
    std::function<void()> onClick;
    std::function<void()> onFocus;
    std::function<void()> onBlur;
};

struct InteractiveComponent : public UIComponent {
    bool isPressed{false};
    bool isHovered{false};
    float animationProgress{0.0f};
};

struct ButtonComponent {
    std::function<void()> onClick;
    bool isHovered{false};
    bool isPressed{false};
    Actor textEntity{entt::null};
    float textOffset{0.0f};  // Stores calculated text offset for proper centering

    ButtonComponent() = default;
    explicit ButtonComponent(std::function<void()> clickHandler, Actor textEnt = entt::null)
        : onClick(std::move(clickHandler)), textEntity(textEnt) {}
};*/