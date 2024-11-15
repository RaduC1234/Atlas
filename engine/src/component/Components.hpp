#pragma once

#include <glm/glm.hpp>

#include "renderer/Sprite.hpp"
#include "renderer/Shapes.hpp"


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