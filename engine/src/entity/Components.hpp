#pragma once

#include "core/Core.hpp"

#include <glm/glm.hpp>

enum EntityCode : uint32_t {
    NEXT = 10000,
    TILE_CODE = 10000,
};

struct NetworkComponent {
    uint64_t networkId;
    uint32_t tileCode; // code, x, x, x -> 1 for tiles xxx for number
    glm::vec3 lastServerPosition;
    mutable bool dirtyFlag;
    mutable bool reload;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(NetworkComponent, networkId, tileCode, dirtyFlag);
};

struct TransformComponent {
    glm::vec3 position;
    float rotation;
    glm::vec2 scale;

    TransformComponent(const glm::vec3 &position, float rotation, const glm::vec2 &scale)
        : position(position),
          rotation(rotation),
          scale(scale) {
    }

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(TransformComponent, position, rotation, scale);
};

struct RenderComponent {
    using TextureCoords = std::array<glm::vec2, 4>;

    bool isCentered; // Origin is centered or starts from the left side
    uint32_t shape; // 0 for quads, 1 for circles, 2 for text
    std::string textureKey; // Background texture for quads
    glm::vec4 color; // Primary color for quads or text
    glm::vec4 borderColor; // Border color (optional)
    float borderWidth; // Border width (optional)
    std::string text; // Rendered text (if shape == 2)
    std::string fontKey; // Font key for text rendering
    TextureCoords coords; // Texture coordinates for quads
    int32_t renderFlags;

    // Constructor for quad rendering
    RenderComponent(const std::string &texture_key,
                    const TextureCoords coords = defaultTexCoords(),
                    const glm::vec4 &color = glm::vec4(1.0f),
                    bool centered = false,
                    int32_t renderFlags = 0)
        : textureKey(texture_key),
          coords(coords),
          color(color),
          shape(0), // Quad shape
          isCentered(centered),
          renderFlags(renderFlags) {
    }

    // Constructor for text rendering
    RenderComponent(const std::string &font_key,
                    const std::string &text,
                    bool centered = false,
                    const glm::vec4 &color = glm::vec4(1.0f)
    )
        : fontKey(font_key),
          text(text),
          color(color),
          shape(2), // Text shape
          isCentered(centered) {
    }

    constexpr static TextureCoords defaultTexCoords() {
        return {
            glm::vec2(0, 1),
            glm::vec2(1, 1),
            glm::vec2(1, 0),
            glm::vec2(0, 0)
        };
    }

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(RenderComponent, textureKey, coords, color, borderColor, borderWidth, shape, text, fontKey, isCentered);
};

struct PawnComponent {
    uint64_t playerId{0};
    bool moveForward{false};
    bool moveBackwards{false};
    bool moveLeft{false};
    bool moveRight{false};
    float aimRotation{0};

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(PawnComponent, playerId);
};

struct RigidbodyComponent {
    bool isSolid{false};
};

//=======================UI============================
struct TextboxComponent {
    std::string text; // Reference to external string
    std::string displayText;
    std::string font;
    glm::vec4 textColor;
    size_t cursorPosition;
    bool isFocused; // Whether the textbox is active
    bool multiline; // Support for multiline text
    bool isPassword;
    size_t maxLength;

    TextboxComponent(std::string textRef,
                     std::string font,
                     glm::vec4 textColor,
                     size_t maxLength = 0,
                     size_t cursorPos = 0,
                     bool focused = false,
                     bool multiline = false,
                     bool isPassword = false)
        : text(textRef),
          font(font),
          textColor(textColor),
          maxLength(maxLength),
          cursorPosition(cursorPos),
          isFocused(focused),
          multiline(multiline),
          isPassword(isPassword) {
        updateDisplayText();
    }

    bool canAddCharacter() const {
        return maxLength == 0 || text.length() < maxLength;
    }

    void updateDisplayText() {
        if (isPassword) {
            displayText = std::string(text.length(), '*');
        } else {
            displayText = text;
        }
    }

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(TextboxComponent, cursorPosition, isFocused, multiline, isPassword);
};


struct ButtonComponent {
    std::string text;
    std::string font;
    glm::vec4 normalTextColor;
    glm::vec4 hoverTextColor;
    glm::vec4 pressedTextColor;
    glm::vec4 disabledTextColor; // Color when the button is disabled
    bool isHovered;
    bool isPressed;
    glm::vec4 normalColor;
    glm::vec4 hoverColor;
    glm::vec4 pressedColor;
    glm::vec4 disabledColor; // Color when the button is disabled
    bool isDisabled; // New field for disabled state
    std::function<void()> onClick;
    std::function<void()> onHover;
    std::function<void()> onPressed;

    ButtonComponent(std::string textRef,
                   std::string fontRef,
                   glm::vec4 normalTextColor,
                   glm::vec4 hoverTextColor = glm::vec4(0.8f),
                   glm::vec4 pressedTextColor = glm::vec4(0.6f),
                   glm::vec4 disabledTextColor = glm::vec4(0.5f), // Default disabled text color
                   bool hovered = false,
                   bool pressed = false,
                   glm::vec4 normalColor = glm::vec4(1.0f),
                   glm::vec4 hoverColor = glm::vec4(0.9f),
                   glm::vec4 pressedColor = glm::vec4(0.8f),
                   glm::vec4 disabledColor = glm::vec4(0.7f), // Default disabled color
                   bool disabled = false, // Default disabled state
                   std::function<void()> clickHandler = nullptr,
                   std::function<void()> hoverHandler = nullptr,
                   std::function<void()> pressHandler = nullptr)
        : text(std::move(textRef)),
          font(std::move(fontRef)),
          normalTextColor(normalTextColor),
          hoverTextColor(hoverTextColor),
          pressedTextColor(pressedTextColor),
          disabledTextColor(disabledTextColor),
          isHovered(hovered),
          isPressed(pressed),
          normalColor(normalColor),
          hoverColor(hoverColor),
          pressedColor(pressedColor),
          disabledColor(disabledColor),
          isDisabled(disabled),
          onClick(std::move(clickHandler)),
          onHover(std::move(hoverHandler)),
          onPressed(std::move(pressHandler)) {
    }

    ButtonComponent() = default;

    void handleClick() {
        if (isDisabled) return;
        if (onClick) onClick();
    }

    void handleHover() {
        if (isDisabled) return;
        if (onHover) onHover();
    }

    void handlePress() {
        if (isDisabled) return;
        if (onPressed) onPressed();
    }


    NLOHMANN_DEFINE_TYPE_INTRUSIVE(ButtonComponent, isHovered, isPressed, isDisabled);
};

struct DisabledComponent {
    bool isDisabled = false;
};
