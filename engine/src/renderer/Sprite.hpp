#pragma once

#include "core/Core.hpp"

#include "Texture.hpp"
#include "glm/vec2.hpp"

using TextureCoords = std::array<glm::vec2, 4>;

struct Sprite {

    // this is for when there is no texture applied to the object
    Sprite() {
        this->texture = nullptr;
        this->texCoords = defaultTexCoords();
    }

    Sprite(Ref<Texture> texture, const TextureCoords &texCoors = defaultTexCoords()) : texture(std::move(texture)), texCoords(texCoors) {}

    Sprite(const std::string &texKey, const TextureCoords &texCoords = defaultTexCoords());

    Ref<Texture> texture;
    TextureCoords texCoords = defaultTexCoords();


    constexpr static TextureCoords defaultTexCoords() {
        return {
            glm::vec2(0, 1),
            glm::vec2(1, 1),
            glm::vec2(1, 0),
            glm::vec2(0, 0)
        };
    }
};
