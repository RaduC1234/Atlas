#pragma once

#include <glm/glm.hpp>

#include "resource/Animation.hpp"

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
    uint32_t playerId{0};
    bool moveForward{false};
    bool moveBackwards{false};
    bool moveLeft{false};
    bool moveRight{false};
    float aimRotation{0};

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(PawnComponent, playerId);
};

struct RigidBodyComponent {
    bool useIncremental{true};
};

struct TextboxComponent {
    std::string text; // Reference to external string
    std::string font;
    glm::vec4 textColor;
    size_t cursorPosition; // Current position of the cursor
    bool isFocused; // Whether the textbox is active
    bool multiline; // Support for multiline text
    size_t maxLength;

    // Constructor to initialize the reference
    TextboxComponent(std::string textRef,
                     std::string font,
                     glm::vec4 textColor,
                     size_t maxLength = 0,
                     size_t cursorPos = 0,
                     bool focused = false,
                     bool multiline = false)
        : text(textRef),
          font(font),
          textColor(textColor),
          maxLength(maxLength),
          cursorPosition(cursorPos),
          isFocused(focused),
          multiline(multiline) {
    }

    bool canAddCharacter() const {
        return maxLength == 0 || text.length() < maxLength;
    }

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(TextboxComponent, cursorPosition, isFocused, multiline);
};


struct ButtonComponent {
    std::string text;
    std::string font;
    glm::vec4 textColor;
    bool isHovered;
    bool isPressed;
    glm::vec4 normalColor;
    glm::vec4 hoverColor;
    glm::vec4 pressedColor;
    std::function<void()> onClick;
    std::function<void()> onHover;
    std::function<void()> onPressed;

    // Constructor to initialize the reference
    ButtonComponent(std::string textRef,
                   std::string fontRef,
                   glm::vec4 color,
                   bool hovered = false,
                   bool pressed = false,
                   std::function<void()> clickHandler = nullptr,
                   std::function<void()> hoverHandler = nullptr,
                   std::function<void()> pressHandler = nullptr,
                   glm::vec4 normalColor = glm::vec4(1.0f),
                   glm::vec4 hoverColor = glm::vec4(0.9f, 0.9f, 0.9f, 1.0f),
                   glm::vec4 pressedColor = glm::vec4(0.8f, 0.8f, 0.8f, 1.0f))
        : text(std::move(textRef)),
          font(std::move(fontRef)),
          textColor(color),
          isHovered(hovered),
          isPressed(pressed),
          onClick(std::move(clickHandler)),
          onHover(std::move(hoverHandler)),
          onPressed(std::move(pressHandler)),
          normalColor(normalColor),
          hoverColor(hoverColor),
          pressedColor(pressedColor){
    }

    ButtonComponent() = default;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(ButtonComponent, isHovered, isPressed);
};



//=========================================================================================
struct AnimationComponent {
    std::shared_ptr<Animation> currentAnimation;
    std::unordered_map<std::string, std::shared_ptr<Animation> > animations;
    float animationTimer = 0.0f;
    size_t currentFrame = 0;
    bool paused = false;

    void update(double deltaTime) {
        if (paused || !currentAnimation) return;

        animationTimer += deltaTime;
        if (animationTimer >= currentAnimation->getFrameDuration()) {
            animationTimer = 0.0f;
            currentFrame = (currentFrame + 1) % currentAnimation->getFrames().size();
        }
    }

    std::string getCurrentFrame() const {
        if (!currentAnimation) {
            std::cerr << "Warning: No current animation is set.\n";
            return "";
        }
        const auto &frames = currentAnimation->getFrames();
        if (frames.empty()) {
            std::cerr << "Warning: Current animation '" << currentAnimation->getName() << "' has no frames.\n";
            return "";
        }
        return frames[currentFrame];
    }

    void setAnimation(const std::string &animationKey) {
        if (animations.find(animationKey) != animations.end()) {
            currentAnimation = animations[animationKey];
            animationTimer = 0.0f;
            currentFrame = 0;
        } else {
            std::cerr << "Warning: Animation '" << animationKey << "' not found.\n";
        }
    }

    void pause() { paused = true; }
    void resume() { paused = false; }

    void validateAnimations() const {
        for (const auto &[key, anim]: animations) {
            if (!anim) {
                std::cerr << "Error: Animation '" << key << "' is null.\n";
            } else if (anim->getFrames().empty()) {
                std::cerr << "Warning: Animation '" << key << "' has no frames.\n";
            }
        }
    }
};
