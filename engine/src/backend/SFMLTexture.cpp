#include "SFMLTexture.hpp"

SFMLTexture::SFMLTexture(const std::string& filePath) {
    if(!texture.loadFromFile(filePath)){
        AT_ERROR("Failed loading texture: {0}", filePath)
    }
}
