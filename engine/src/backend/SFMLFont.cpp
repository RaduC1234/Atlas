#include "SFMLFont.hpp"

SFMLFont::SFMLFont(const std::string &filePath) {
    font.loadFromFile(filePath);
}

void *SFMLFont::getNativeResource() {
    return reinterpret_cast<void*>(&this->font);
}


