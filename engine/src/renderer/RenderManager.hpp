
#pragma once

#include "Renderer.hpp"

class RenderManager {
public:
    static void init(uint32_t maxBatchSize = 1000);

    static void shutdown();


    /**
    * Draws a quad on the screen.
    * @param position x and y represent the position of the quad on the screen, z is the z-index of the quad
    * @param size size of the quad
    * @param rotation rotation of the quad in degrees
    * @param color color of the quad
    * @param sprite leave null for opaque quad
    */
    static void drawRotatedQuad(const glm::vec3 &position, const glm::vec2 &size, float rotation, const glm::vec4 &color, const Sprite& sprite = Sprite(nullptr), bool centered = false);

    /**
    * Draws a quad on the screen.
    * @param position x and y represent the position of the quad on the screen, z is the z-index of the quad
    * @param color color of the quad
    * @param sprite leave null for opaque quad
    */
    static void drawQuad(const glm::vec3 &position, const glm::vec2 &size, const glm::vec4 &color, const Sprite& sprite = Sprite(nullptr), bool centered = false);

    /**
    * Draws a text on the screen.
    * @param position x and y represent the position of the text on the screen, z is the z-index of the text
    * @param text text to drawPrimitive
    * @param font font to use
    * @param size size of the glyphs
    * @param color color of the text
    */
    static void drawText(const glm::vec3 &position, const std::string &text, const Ref<Font> &font, float size, const glm::vec4 &color, bool centered = false);

    /**
     * Draws a Circle on the screen.
     * @param position x and y represent the position of the line on the screen, z is the z-index of the circle
     * @param radius radius of the circle
     * @param color color of the circle
     * @param sprite leave null for opaque circle
     */

    static void drawCircle(const glm::vec3 &position, float radius, const glm::vec4 &color, const Sprite &sprite);

    static void flush(uint32_t screenWidth, uint32_t screenHeight, Camera &camera);

private:
    static Renderer renderer;
};

