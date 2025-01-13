#include "Renderer.hpp"

Renderer::Renderer() {
}

Renderer::Renderer(uint32_t maxBatchSize, const glm::vec4 clearColor): maxBatchSize(maxBatchSize), clearColor(clearColor) {
    if (!initialized) {
        Renderer::init();
        initialized = true;
    }

    this->frameBuffer = Framebuffer(800, 600);
}

void Renderer::drawPrimitive(const glm::vec3 &position, const glm::vec2 &scale, float rotation, Shape shape, const glm::vec4 &color, const Ref<Texture> &texture, const TextureCoords &texCoords, bool centered, int32_t properties) {
    float zIndex = position.z;

    bool added = false;
    for (auto &x: batches) {
        if (!x.isFull() && x.getZIndex() == zIndex) {
            // if quad has no texture
            if (texture == nullptr || (x.hasTexture(texture) || x.hasTextureRoom())) {
                x.addShape(position, scale, rotation, shape, color, texture, texCoords, centered, properties);
                added = true;
                break;
            }
        }
    }

    if (!added) {
        batches.emplace_back(maxBatchSize, renderShader, zIndex);
        batches.back().addShape(position, scale, rotation, shape, color, texture, texCoords, centered, properties);
    }
}

void Renderer::drawText(glm::vec3 position, float scale, const glm::vec4 &color, const Font &font, const std::string &text, bool centered) {
    /*
     * https://learnopengl.com/In-Practice/Text-Rendering
     * https://steamcdn-a.akamaihd.net/apps/valve/2007/SIGGRAPH2007_AlphaTestedMagnification.pdf
     */
    if (centered) {
        float totalWidth = 0.0f;
        for (const char c: text) {
            const auto &[textureID, size, bearing, advance] = font.getCharacter(c);
            totalWidth += (advance / 64.0f) * scale;
        }
        totalWidth -= (font.getCharacter(text.back()).advance / 64.0f) * scale; // Remove extra space after the last character

        float totalHeight = 0.0f;
        for (const char c: text) {
            const auto &[textureID, size, bearing, advance] = font.getCharacter(c);
            float height = static_cast<float>(size.y) * scale;
            totalHeight = std::max(totalHeight, height);
        }

        position.x -= totalWidth / 2.0f;
        position.y += totalHeight / 2.0f;
    }


    for (const char c: text) {
        const auto &[textureID, size, bearing, advance] = font.getCharacter(c);

        float xpos = position.x + bearing.x * scale;
        float ypos = position.y - (size.y - bearing.y) * scale;

        float width = static_cast<float>(size.x) * scale;
        float height = static_cast<float>(size.y) * scale;

        static std::unordered_map<uint32_t, Ref<Texture> > textureCache;
        if (textureCache.find(textureID) == textureCache.end()) {
            textureCache[textureID] = CreateRef<Texture>(textureID);
        }
        auto sprite = Sprite(textureCache[textureID]);

        drawPrimitive(
            glm::vec3(xpos, ypos, position.z),
            glm::vec2(width, height),
            0.0f,
            Shape::TEXT,
            color,
            sprite.texture,
            sprite.texCoords,
            false,
            0x0
        );

        position.x += (advance / 64.0f) * scale;
    }
}

void Renderer::flush(uint32_t screenWidth, uint32_t screenHeight, Camera &camera) {
    camera.applyViewport(screenWidth, screenHeight);

    // Sort only if needed
    static bool sorted = false;
    if (!sorted) {
        std::ranges::sort(batches, [](const RenderBatch &a, const RenderBatch &b) {
            return a.getZIndex() > b.getZIndex();
        });
        sorted = true;
    }

    glClearColor(clearColor.x, clearColor.y, clearColor.z, clearColor.w);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for (auto &batch: batches) {
        batch.start();
        batch.render(screenWidth, screenHeight, camera);
        batch.clear();
    }

    sorted = false;

    GLenum err;
    if ((err = glGetError()) != GL_NO_ERROR) {
        AT_ERROR("OpenGL error: {0}", err);
    }
}


void Renderer::init() {
    // link glfw and glad
    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
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
    postprocessingShader = CreateRef<Shader>("assets/shaders/fragment_full.glsl");
}

Ref<Shader> Renderer::renderShader;
Ref<Shader> Renderer::postprocessingShader;
bool Renderer::initialized{false};