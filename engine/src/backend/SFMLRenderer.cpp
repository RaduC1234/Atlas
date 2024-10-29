#include "SFMLRenderer.hpp"

SFMLRenderer::SFMLRenderer(SFMLWindow *window, glm::vec4 clearColor)
        : window(std::any_cast<std::reference_wrapper<sf::RenderWindow>>(window->getNativeWindow()).get()), // fac asa ca sa evit void* chiar daca e mai usor c style
          clearColor(clearColor) {

}

void SFMLRenderer::drawQuad(const glm::vec3 &position, const glm::vec2 &size, float rotation, const glm::vec4 &color, const Sprite &sprite, bool normalized) {
    sf::RectangleShape quad;
    quad.setSize(sf::Vector2f(size.x, size.y));
    quad.setFillColor(sf::Color(color.r * 255, color.g * 255, color.b * 255, color.a * 255));
    quad.setOrigin(size.x / 2.0f, size.y / 2.0f);
    quad.setPosition(position.x, position.y);
    quad.setRotation(rotation);
}

void SFMLRenderer::drawText(const glm::vec3 &position, const std::string &text, const Ref<Font> &font, float size, float spacing, const glm::vec4 &color, bool normalized) {

}

void SFMLRenderer::drawCircle(const glm::vec3 &position, float radius, const glm::vec4 &color, const Sprite &sprite, bool normalized) {

}

void SFMLRenderer::flush(uint32_t screenWidth, uint32_t screenHeight, Camera &camera) {

}


