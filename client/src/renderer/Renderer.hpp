#pragma once

#include "Font.hpp"
#include "Framebuffer.hpp"
#include "RenderBatch.hpp"
#include "Sprite.hpp"
#include <glad/glad.h>
#include <GLFW/glfw3.h> // make this the last include

#include "Shapes.hpp"


class Renderer {
public:
    Renderer();

    Renderer(uint32_t maxBatchSize, const glm::vec4 clearColor = {0.0863f, 0.0863f, 0.0863f, 1.0f});

    /**
     * Adds a primitive shape to the draw queue. This method attempts to locate an existing batch
     * in the batch slot that uses the same texture, allowing the shape to be added efficiently.
     *
     * <p>Supported shapes include:
     * <ul>
     *   <li>Quads</li>
     *   <li>Circles</li>
     *   <li>Text</li>
     * </ul>
     *
     * @note <p>All shapes are sent to the GPU as quads, with their unique appearance handled in the
     * fragment shader at render time.
     *
     * @param position to draw to.
     * @param shape the shape to add to the draw queue
     * @param texture the texture associated with the shape, used for batch matching
     * @param centered specifies if the texture is to be drawn from centered if true, of from left bottom corner otherwise
     */
    void drawPrimitive(const glm::vec3 &position, const glm::vec2 &scale, float rotation, Shape shape, const glm::vec4 &color, const Ref<Texture> &texture, const TextureCoords &texCoords, bool centered, int32_t properties);

    void drawText(glm::vec3 position, float scale, const glm::vec4 &color, const Font &font, const std::string &text, bool centered = false);

    void flush(uint32_t screenWidth, uint32_t screenHeight, Camera &camera);

    void static init();

private:
    uint32_t maxBatchSize = 0;
    std::vector<RenderBatch> batches;
    glm::vec4 clearColor{1.0f, 1.0f, 1.0f, 1.0f};

    Framebuffer frameBuffer{};

    static Ref<Shader> renderShader;
    static Ref<Shader> postprocessingShader;
    static bool initialized;
};
