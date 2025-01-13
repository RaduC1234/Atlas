#pragma once

#include "Texture.hpp"
#include "Shader.hpp"
#include "Camera.hpp"

#define RENDERER_NINE_SLICE BIT(0)

/**
 * The RenderBatch class manages a collection of 2D shapes, textures, and rendering data,
 * and it organizes them into a single batch for efficient rendering. By batching multiple
 * shapes together, the RenderBatch reduces the number of draw calls and improves rendering
 * performance.
 *
 * <p>Each RenderBatch holds a fixed number of vertices and indices (determined by maxBatchSize)
 * and stores texture references used by the shapes. Shapes within a batch are rendered in a
 * single draw call, provided they use compatible textures. The class also handles setting up
 * OpenGL objects like VAO (Vertex Array Object), VBO (Vertex Buffer Object), and EBO
 * (Element Buffer Object) to manage vertex and index data.
 *
 * <p>Each shape added to the RenderBatch is represented as a quad, and these quads can be
 * customized as rectangles, circles, or text by setting unique properties that the fragment
 * shader interprets during rendering. The RenderBatch also tracks when it becomes full to
 * signal the need for a new batch.
 */
class RenderBatch {
public:
    RenderBatch() = default;

    RenderBatch(int32_t maxBatchSize, Ref<Shader> quadShader, float_t zIndex);

    ~RenderBatch();


    void start();


    void addShape(const glm::vec2 &position, const glm::vec2 &scale, float rotation, uint32_t shape, const glm::vec4 &color, const Ref<Texture> &texture, const std::array<glm::vec2, 4> &texCoords, bool centered = true, int32_t properties = 0);


    void render(int screenWidth, int screenHeight, Camera &camera);

    bool hasTextureRoom() const {
        return textures.size() < sizeof(texSlots) / sizeof(int) - 1; // for f**k’s sake, this bug took me 5 days
    }

    bool hasTexture(const Ref<Texture> &texture) {
        return std::ranges::find(textures, texture) != textures.end();
    }

    bool isFull() const {
        return full;
    }

    float getZIndex() const {
        return zIndex;
    }

    void clear();

private:

    struct Vertex {
        glm::vec3 position;
        glm::vec4 color;
        glm::vec2 texCoords;
        float_t texID;
        float_t shape;
    };

    uint32_t maxBatchSize = 0;
    float_t zIndex{};
    bool full = false;

    uint32_t VAO{}, VBO{}, EBO{};

    std::vector<Vertex> vertices;
    uint32_t vertexIndex = 0; // holds the drawing index in the element array
    std::vector<uint32_t> indices;

    Ref<Shader> shader;
    std::vector<Ref<Texture>> textures;
    int texSlots[16] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
};
