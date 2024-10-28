#pragma once

#include <utility>

#include "core/Core.hpp"

#include "Texture.hpp"
#include <glm/glm.hpp>

using TextureCoords = std::array<glm::vec2, 4>;

struct Sprite {
public:
    Sprite(const Ref<Texture>& texture);

    Sprite(const Ref<Texture>& texture, TextureCoords texCoors);

    Ref<Texture> texture;
    TextureCoords texCoords;
};

class SpriteSheet {
public:
    SpriteSheet(const std::string& texturePath, const std::string& despriptorPath);
};
