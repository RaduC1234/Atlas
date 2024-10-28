#pragma once

#include "renderer/Font.hpp"
#include "renderer/Texture.hpp"
#include "renderer/Renderer.hpp"

class ResourceBundle {
public:

private:
    Renderer *renderer;
    std::unordered_map<int, Texture> textures;
    std::unordered_map<int, Font> font;
};

