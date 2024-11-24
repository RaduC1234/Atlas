#include <functional>

struct UIComponent {
    enum class Type {
        Button,
        Text,
        Checkbox,
        Slider
    };

    Type type;
    std::function<void()> onClick; // Callback for interaction

    bool isHovered = false;  // To track hover state
    bool isPressed = false;  // To track pressed state

    UIComponent(Type type, std::function<void()> callback = nullptr)
        : type(type), onClick(std::move(callback)) {}
};
