#pragma once

#include "core/Core.hpp"

#include "Texture.hpp"
#include "glm/vec2.hpp"

using TextureCoords = std::array<glm::vec2, 4>;

class Sprite {
public:

    Ref<Texture> texture;

    TextureCoords texCoords = { // image flipped on x to be displayed correctly
            glm::vec2(0, 1),
            glm::vec2(1, 1),
            glm::vec2(1, 0),
            glm::vec2(0, 0)
    };

    // this is for when there is no texture applied to the object
    Sprite() = default;

    Sprite(Ref<Texture> texture) : texture(std::move(texture)) {}

    Sprite(Ref<Texture> texture, std::array<glm::vec2, 4> texCoors, int index) : texture(std::move(texture)), texCoords(texCoors) {}

};
