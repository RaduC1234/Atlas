#pragma once

#include "renderer/Renderer.hpp"
#include "SFMLWindow.hpp"

class SFMLRenderer : public Renderer {
public:

    SFMLRenderer(SFMLWindow* window, glm::vec4 clearColor);

    void drawQuad(const glm::vec3 &position, const glm::vec2 &size, float rotation, const glm::vec4 &color, const Sprite &sprite, bool normalized) override;

    void drawText(const glm::vec3 &position, const std::string &text, const Ref<Font> &font, float size, float spacing, const glm::vec4 &color, bool normalized) override;

    void drawCircle(const glm::vec3 &position, float radius, const glm::vec4 &color, const Sprite &sprite, bool normalized) override;

    void flush(uint32_t screenWidth, uint32_t screenHeight, Camera &camera) override;

private:
    glm::vec4 clearColor;
    const sf::RenderWindow& window;
};

