#pragma once

#include "Font.hpp"
#include "Framebuffer.hpp"
#include "RenderBatch.hpp"
#include "Sprite.hpp"

#include <GLFW/glfw3.h> // make this the last include


enum Shape : uint32_t {
    QUAD,
    CIRCLE,
    TEXT
};

class Renderer {
public:
    Renderer() = default;

    Renderer(uint32_t maxBatchSize, const glm::vec4 clearColor = {0.0863f, 0.0863f, 0.0863f, 1.0f}) : maxBatchSize(maxBatchSize), clearColor(clearColor) {
        if (!initialized) {
            Renderer::init();
            initialized = true;
        }

        this->frameBuffer = Framebuffer(800, 600);
    }

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
     * @param postition to draw to.
     * @param shape the shape to add to the draw queue
     * @param texture the texture associated with the shape, used for batch matching
     */
    void drawPrimitive(const glm::vec3 &position, const glm::vec2 &scale, float rotation, Shape shape, const glm::vec4 &color, const Ref<Texture> &texture, const TextureCoords &texCoords, bool centered = true) {

        float zIndex = position.z;

        bool added = false;
        for (auto &x: batches) {
            if (!x.isFull() && x.getZIndex() == zIndex) {

                // if quad has no texture
                if (texture == nullptr || (x.hasTexture(texture) || x.hasTextureRoom())) {
                    x.addShape(position, scale, rotation, shape, color, texture, texCoords);
                    added = true;
                    break;
                }
            }
        }

        if (!added) {
            batches.emplace_back(maxBatchSize, renderShader, zIndex);
            batches.back().addShape(position, scale, rotation, shape, color, texture, texCoords);
        }

    }

    void drawQuad(const glm::vec3 position, const glm::vec2 size, const glm::vec4 color, const Sprite &sprite = Sprite(nullptr)) {
        drawPrimitive(position, size, 0.0f, Shape::QUAD, color, sprite.texture, sprite.texCoords);
    }

    void drawRotatedQuad(const glm::vec3 position, const glm::vec2 size, float rotation, const glm::vec4 color, const Sprite &sprite = Sprite(nullptr)) {
        drawPrimitive(position, size, rotation, Shape::QUAD, color, sprite.texture, sprite.texCoords);
    }

    void drawText(glm::vec3 position, float scale, const glm::vec4& color, const Font &font, const std::string &text) {

       // https://learnopengl.com/In-Practice/Text-Rendering
        for (char c: text) {

            const Character &character = font.getCharacter(c);

            float xpos = position.x + character.bearing.x * scale;
            float ypos = position.y - (character.size.y - character.bearing.y) * scale;


            float width = character.size.x * scale;
            float height = character.size.y * scale;

            auto sprite = Sprite(CreateRef<Texture>(character.textureID)); // this is bad for memory

            drawPrimitive(
                    glm::vec3(xpos, ypos, position.z),
                    glm::vec2(width, height),
                    0.0f,
                    Shape::TEXT,
                    color,
                    sprite.texture,
                    sprite.texCoords
            );

            // Advance the position for the next character (considering the scale)
            position.x += (character.advance / 64.0f) * scale;  // Correctly scale the advance (1/64th of a pixel)
        }
    }


    void flush(uint32_t screenWidth, uint32_t screenHeight, Camera &camera) {

        // sort the batches by their z-index so the transparency is applied correctly
        std::sort(batches.begin(), batches.end(),
                  [](const RenderBatch &a, const RenderBatch &b) {
                      return a.getZIndex() > b.getZIndex();
                  });

        glClearColor(clearColor.x, clearColor.y, clearColor.z, clearColor.w); // clear the screen and apply the background color
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear the color buffer and depth buffer

        for (auto &batch: batches) {
            batch.start();
            batch.render(screenWidth, screenHeight, camera);
        }
        batches.clear();

        GLenum err;
        if ((err = glGetError()) != GL_NO_ERROR) {
            AT_ERROR("OpenGL error: {0}", err);
        }
    }

    void static init() {

        // link glfw and glad
        if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
            AT_FATAL("Failed to initialize GLAD");
        }

        // get the number of textures loadable in a single batch by the gpu
        int textureUnits;
        glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &textureUnits);
        AT_INFO("Texture units available on hardware: {0}.", textureUnits);


        glEnable(GL_CULL_FACE); // Cull faces behind for performance
        glEnable(GL_BLEND); // enable transparency
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // Enable Anti-Aliasing - todo: implement with framebuffer - also check window class when removing this, line 40
        glEnable(GL_MULTISAMPLE);

        // Initialize FreeType
        FT_Library ft;
        if (FT_Init_FreeType(&ft)) {
            std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
            return;
        }

        renderShader = CreateRef<Shader>("assets/shaders/render.glsl");
    }

private:
    uint32_t maxBatchSize = 0;
    std::vector<RenderBatch> batches;
    glm::vec4 clearColor{1.0f, 1.0f, 1.0f, 1.0f};

    Framebuffer frameBuffer;

    inline static Ref<Shader> renderShader;
    inline static Ref<Shader> postprocessingShader;
    inline static bool initialized = false;
};
