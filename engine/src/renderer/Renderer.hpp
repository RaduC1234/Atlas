#pragma once

#include "core/Core.hpp"

#include <glm/glm.hpp>

#include "Font.hpp"
#include "Sprite.hpp"
#include "Camera.hpp"
#include "core/Window.hpp"


class Renderer {
public:
    virtual ~Renderer() = default;

    /**
     * Draws a quad on the screen.
     * @param position x and y represent the position of the quad on the screen, z is the z-index of the quad
     * @param size size of the quad
     * @param rotation rotation of the quad in degrees
     * @param color color of the quad
     * @param sprite leave null for opaque quad
    * @param normalized indicates if the quad is drawn in screen space (1) or world space (0). The screen coords are mapped to window width and height.
    */
    virtual void drawQuad(const glm::vec3 &position, const glm::vec2 &size, float rotation, const glm::vec4 &color, const Sprite &sprite, bool normalized) = 0;

    /**
     * Draws a text on the screen.
     * @param position x and y represent the position of the text on the screen, z is the z-index of the text
     * @param text text to draw
     * @param font font to use
     * @param size size of the glyphs
     * @param spacing spacing between the glyphs
     * @param color color of the text
     * @param normalized indicates if the text is drawn in screen space (1) or world space (0). The screen coords are mapped to window width and height.
     */
    virtual void drawText(const glm::vec3 &position, const std::string &text, const Ref<Font> &font, float size, float spacing, const glm::vec4 &color, bool normalized) = 0;

    /**
     * Draws a Circle on the screen.
     * @param position x and y represent the position of the line on the screen, z is the z-index of the circle
     * @param radius radius of the circle
     * @param color color of the circle
     * @param sprite leave null for opaque circle
     * @param normalized indicates if the circle is drawn in screen space (1) or world space (0). The screen coords are mapped to window width and height.
     */
    virtual void drawCircle(const glm::vec3 &position, float radius, const glm::vec4 &color, const Sprite &sprite, bool normalized) = 0;

    virtual void flush(uint32_t screenWidth, uint32_t screenHeight, Camera& camera) = 0;

	static Scope<Renderer> create(void* nativeWindow, glm::vec4 clearColor = {0.0863f, 0.0863f, 0.0863f, 1.0f});

};
