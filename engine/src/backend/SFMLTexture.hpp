#pragma once

#include <SFML/Graphics/Texture.hpp>
#include "renderer/Texture.hpp"

/**
 * Wrapper class for SFML Texture
 */
class SFMLTexture : public Texture {
public:

    SFMLTexture(const std::string& filePath);

    uint32_t getWidth() override { return this->texture.getSize().x; }

    uint32_t getHeight() override { return this->texture.getSize().y; }

    void *getNativeTexture() override { return reinterpret_cast<void *>(&texture); }

private:
    sf::Texture texture;
};



