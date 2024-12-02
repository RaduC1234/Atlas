#include "Sprite.hpp"

#include "resource/ResourceManager.hpp"

Sprite::Sprite(const std::string &texKey, const TextureCoords &texCoords) {
    this->texture = ResourceManager::get<Texture>(texKey);
    this->texCoords = texCoords;
}
