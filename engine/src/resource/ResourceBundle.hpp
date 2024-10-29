#pragma once

#include "renderer/Font.hpp"
#include "renderer/Texture.hpp"
#include "renderer/Renderer.hpp"

class ResourceBundle {
public:

private:
    // index by filepath
    std::unordered_map<std::string, Texture> textures;
    std::unordered_map<std::string, Font> font;
};

