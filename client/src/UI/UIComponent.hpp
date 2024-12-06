#pragma once

#include <functional>

struct UIComponent {
    enum class Type {
        Button,
        Text,
        Checkbox,
        Slider
    };

    Type type;
    bool isHovered = false;  // To track hover state
    bool isPressed = false;  // To track pressed state

    // Virtual destructor for polymorphism
    virtual ~UIComponent() = default;

    // Optional: Pure virtual methods for common interactions
    virtual void onHover() {}
    virtual void onPress() {}
    virtual void onRelease() {}
};
