#include "Sprite.hpp"

Sprite::Sprite(const Ref<SFMLTexture> &texture) { // image flipped on x to be displayed correctly
    this->texture = texture;
    this->texCoords = {
            glm::vec2(1, 0),
            glm::vec2(1, 1),
            glm::vec2(0, 1),
            glm::vec2(0, 0)
    };
}

Sprite::Sprite(const Ref<SFMLTexture> &texture, TextureCoords texCoors) {
    this->texture = texture;
    this->texCoords = texCoors;
}
