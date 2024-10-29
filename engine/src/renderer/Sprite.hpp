#pragma once

#include <utility>

#include "core/Core.hpp"

#include "Texture.hpp"
#include "backend/SFMLTexture.hpp"
#include <glm/glm.hpp>

using TextureCoords = std::array<glm::vec2, 4>;

struct Sprite {
public:
    Sprite(const Ref<SFMLTexture>& texture);

    Sprite(const Ref<SFMLTexture>& texture, TextureCoords texCoors);

    Ref<SFMLTexture> texture;
    TextureCoords texCoords;
};

class SpriteSheet {
public:
    SpriteSheet(const std::string& texturePath, const std::string& despriptorPath);
};
