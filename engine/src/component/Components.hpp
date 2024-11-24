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

    RenderComponent(const std::string &text, std::string font_key)
        : shape(Shape::TEXT),
          text(text),
          fontKey(std::move(font_key)){
    }
};

struct PawnComponent {
    uint32_t serverId;
};

struct AnimationComponent
{
    std::shared_ptr<Animation> currentAnimation;
    float animationTimer = 0.0f;
    size_t currentFrame = 0;

    void update(double deltaTime) {
        if (!currentAnimation) return;

        animationTimer += deltaTime;
        if (animationTimer >= currentAnimation->getFrameDuration()) {
            animationTimer = 0.0f;
            currentFrame = (currentFrame + 1) % currentAnimation->getFrames().size();
        }
    }

    std::string getCurrentFrame() const {
        if (!currentAnimation) return "";
        const auto& frames = currentAnimation->getFrames();
        if (frames.empty()) return "";
        return frames[currentFrame];
    }
};