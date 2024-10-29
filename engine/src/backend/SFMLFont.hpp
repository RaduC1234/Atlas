#pragma once

#include <SFML/Graphics/Font.hpp>
#include "renderer/Font.hpp"

class SFMLFont : public Font {
public:

    SFMLFont(const std::string& filePath) ;

    void *getNativeResource() override;

private:
    sf::Font font;
};
