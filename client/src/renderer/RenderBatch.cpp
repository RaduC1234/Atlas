#include "RenderBatch.hpp"

#include <Atlas.hpp>
#include <glad/glad.h>

RenderBatch::RenderBatch(int32_t maxBatchSize, Ref<Shader> quadShader, float_t zIndex) : maxBatchSize(maxBatchSize),
    shader(std::move(quadShader)), zIndex(zIndex) {
    vertices.reserve(maxBatchSize * 4); // 4 vertices per quad
    indices.reserve(maxBatchSize * 6); // 6 indices per quad
}

RenderBatch::~RenderBatch() {
    if (VAO)
        glDeleteVertexArrays(1, &VAO);
    if (VBO)
        glDeleteBuffers(1, &VBO);
    if (EBO)
        glDeleteBuffers(1, &EBO);
}

void RenderBatch::start() {
    // Create and bind the Vertex Array Object (VAO)
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // Generate and bind the Vertex Buffer Object (VBO)
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), nullptr, GL_DYNAMIC_DRAW);

    // Generate and bind the Element Buffer Object (EBO)
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t), nullptr, GL_DYNAMIC_DRAW);

    // bind position on location 0
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void *>(offsetof(Vertex, position)));
    glEnableVertexAttribArray(0);

    // bind color on location 1
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void *>(offsetof(Vertex, color)));
    glEnableVertexAttribArray(1);

    // bind texture coordinates on location 2
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void *>(offsetof(Vertex, texCoords)));
    glEnableVertexAttribArray(2);

    // bind texID on location 3
    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void *>(offsetof(Vertex, texID)));
    glEnableVertexAttribArray(3);

    // bind shape on location 4
    glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void *>(offsetof(Vertex, shape)));
    glEnableVertexAttribArray(4);

    // bind circle radius on location 5
    glVertexAttribPointer(5, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void *>(offsetof(Vertex, radius)));
    glEnableVertexAttribArray(5);

    glBindBuffer(GL_ARRAY_BUFFER, 0); // Unbind the VBO
    glBindVertexArray(0); // Unbind the VAO
}

void RenderBatch::addShape(const glm::vec2 &position, const glm::vec2 &scale, float rotation, uint32_t shape,
                           const glm::vec4 &color, const Ref<Texture> &texture,
                           const std::array<glm::vec2, 4> &texCoords, bool centered) {
    int texId = 0;

    // Handle texture binding
    if (texture != nullptr) {
        if (std::ranges::find(textures, texture) == textures.end())
            textures.push_back(texture);

        for (int i = 0; i < textures.size(); i++) {
            if (textures[i] == texture) {
                texId = i + 1;
                break;
            }
        }
    }

    float radians = glm::radians(rotation);

    auto rotationMatrix = glm::mat2(
        glm::cos(radians), -glm::sin(radians),
        glm::sin(radians), glm::cos(radians)
    );

    glm::vec2 originOffset(0.0f, 0.0f);
    if (centered) {
        originOffset = glm::vec2(scale.x / 2.0f, scale.y / 2.0f);
    }

    // Define vertices with bottom-left as the origin
    const glm::vec2 verticesPos[4] = {
        rotationMatrix * (glm::vec2(0.0f, 0.0f) - originOffset), // Bottom-left
        rotationMatrix * (glm::vec2(scale.x, 0.0f) - originOffset), // Bottom-right
        rotationMatrix * (glm::vec2(scale.x, scale.y) - originOffset), // Top-right
        rotationMatrix * (glm::vec2(0.0f, scale.y) - originOffset) // Top-left
    };

    // Add transformed vertices to the vertex array with position, color, texture coordinates, texture ID, and shape ID
    for (int i = 0; i < 4; ++i) {
        vertices.emplace_back(glm::vec3{position + verticesPos[i], zIndex}, color, texCoords[i],
                              static_cast<float_t>(texId), static_cast<float_t>(shape));
    }

    // Add indices for the two triangles that form the quad
    indices.insert(indices.end(), {
                       vertexIndex, vertexIndex + 1, vertexIndex + 2,
                       // First triangle (Bottom-left, Bottom-right, Top-right)

                       vertexIndex, vertexIndex + 2, vertexIndex + 3
                       // Second triangle (Bottom-left, Top-right, Top-left)
                   });

    vertexIndex += 4;

    if (vertexIndex >= maxBatchSize)
        full = true;
}

void RenderBatch::render(int screenWidth, int screenHeight, Camera &camera) {
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // wireframe mode
    shader->bind(); // use shader

    shader->uploadMat4f("uWorldProjection", camera.getProjectionMatrix()); // upload uniforms to the gpu
    shader->uploadMat4f("uView", camera.getViewMatrix());
    shader->uploadFloat("uTime", Time::now().toSeconds());

    // upload the textures to GPU memory
    for (int i = 0; i < textures.size(); i++) {
        glActiveTexture(GL_TEXTURE0 + i + 1);
        textures[i]->bind();
    }

    // upload indices of the textures
    shader->uploadIntArray("uTextures", texSlots, 16);

    // bind the VAO
    glBindVertexArray(VAO);

    // bind the VBO
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(Vertex), vertices.data());

    // bind the EBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, indices.size() * sizeof(uint32_t), indices.data());

    // draw to the screen
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

    // clear textures from GPU memory
    for (const auto &texture: textures) {
        texture->unbind();
    }

    glBindVertexArray(0);
}
