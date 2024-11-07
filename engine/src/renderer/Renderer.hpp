#pragma once

#include "RenderBatch.hpp"
#include "Sprite.hpp"

//#include "avalon/utils/AssetPool.hpp"
//#include "FrameBuffer.hpp"

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
    }


    void drawPrimitive(const glm::vec3 &position, const glm::vec2 &scale, float rotation, Shape shape, const glm::vec4 color, const Ref<Texture> &texture, const TextureCoords &texCoords) {

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

    void drawText(glm::vec3 position, float scale, const glm::vec4 color, const Font &font, const std::string &text) {
        // Iterate through each character in the string
        for (char c : text) {
            // Retrieve the character information from the font
            const Character &character = font.getCharacter(c);

            // Calculate the position of each character
            float xpos = position.x + character.bearing.x * scale;
            float ypos = position.y - (character.size.y - character.bearing.y) * scale;

            // Size of each character quad (scaled by the font size)
            float width = character.size.x * scale;
            float height = character.size.y * scale;

            // Create the sprite for the character (assuming character.textureID is already set correctly)
            auto sprite = Sprite(CreateRef<Texture>(character.textureID));

            // Draw the character using sprite texture coordinates
            drawPrimitive(
                    glm::vec3(xpos, ypos, position.z), // Position of the character
                    glm::vec2(width, height),           // Size of the character (scaled)
                    0.0f,                               // Rotation (none in this case)
                    Shape::TEXT,                        // Shape type (text shape)
                    color,                              // Color of the text
                    sprite.texture,                     // Character texture
                    sprite.texCoords                   // Texture coordinates for the character
            );

            // Advance the position for the next character (considering the scale)
            position.x += (character.advance / 64.0f) * scale;  // Correctly scale the advance (1/64th of a pixel)
        }
    }




    void flush(uint32_t screenWidth, uint32_t screenHeight, Camera &camera) {

        std::sort(batches.begin(), batches.end(),
                  [](const RenderBatch &a, const RenderBatch &b) {
                      return a.getZIndex() > b.getZIndex();
                  });

        glClearColor(clearColor.x, clearColor.y, clearColor.z, clearColor.w);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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

        if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
            std::cout << "Failed to initialize GLAD" << std::endl;
            exit(1);
        }

        int textureUnits;
        glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &textureUnits);
        AT_INFO("Texture units available on hardware: {0}.", textureUnits);

        glEnable(GL_CULL_FACE);
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

    //FrameBuffer frameBuffer;

    inline static Ref<Shader> renderShader;
    inline static Ref<Shader> postprocessingShader;
    inline static bool initialized = false;
};
