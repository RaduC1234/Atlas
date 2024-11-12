#pragma once

#include "core/Core.hpp"

#include "Texture.hpp"
#include "glm/vec2.hpp"

using TextureCoords = std::array<glm::vec2, 4>;

struct Sprite {

    // this is for when there is no texture applied to the object
    constexpr Sprite() = default;

    Sprite(Ref<Texture> texture) : texture(std::move(texture)) {}

    Sprite(Ref<Texture> texture, const TextureCoords& texCoors) : texture(std::move(texture)), texCoords(texCoors) {}

    /**
     * Flips the sprite texture on X axis
     */
    void flipX() {

    }

    /**
     * Flips the sprite texture on Y axis
     */
    void flipY() {

    }

public:
    Ref<Texture> texture;

    TextureCoords texCoords = {
            glm::vec2(0, 1),
            glm::vec2(1, 1),
            glm::vec2(1, 0),
            glm::vec2(0, 0)
    };

};
