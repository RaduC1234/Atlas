#pragma once

#include <glm/glm.hpp>

#include "renderer/Sprite.hpp"
#include "renderer/Shapes.hpp"
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
    std::string textureKey;
    TextureCoords coords;
    glm::vec4 color;
    Shape shape; // 0 for quads, 1 for circles, 2 for text

    // if text
    std::string text;
    std::string fontKey;

    RenderComponent(const std::string &texture_key, const TextureCoords &coords, const glm::vec4 &color)
        : textureKey(texture_key),
          coords(coords),
          color(color),
          shape(Shape::QUAD) {
    }

    RenderComponent(std::string font_key, const std::string &text, const glm::vec4 &color)
        : shape(Shape::TEXT),
          text(text),
          color(color),
          fontKey(std::move(font_key)) {
    }

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(RenderComponent, textureKey, coords, color, shape, text, fontKey);
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
