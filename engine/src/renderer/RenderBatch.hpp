#pragma once

#include "Texture.hpp"
#include "Shader.hpp"
#include "Camera.hpp"
#include "Font.hpp"
#include "Color.hpp"

#include "core/PlatformUtils.hpp"

class RenderBatch {
public:
    RenderBatch() = default;

    RenderBatch(int32_t maxBatchSize, Ref<Shader> quadShader, int zIndex) : maxBatchSize(maxBatchSize), shader(std::move(quadShader)), zIndex(zIndex) {
        vertices.reserve(maxBatchSize * 4); // 4 vertices per quad
        indices.reserve(maxBatchSize * 6); // 6 indices per quad
    }

    ~RenderBatch() {
        if (VAO) glDeleteVertexArrays(1, &VAO);
        if (VBO) glDeleteBuffers(1, &VBO);
        if (EBO) glDeleteBuffers(1, &EBO);
    }


    void start() {
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
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, position));
        glEnableVertexAttribArray(0);

        // bind color on location 1
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, color));
        glEnableVertexAttribArray(1);

        // bind texture coordinates on location 2
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, texCoords));
        glEnableVertexAttribArray(2);

        // bind texID on location 3
        glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, texID));
        glEnableVertexAttribArray(3);

        // bind shape on location 4
        glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, shape));
        glEnableVertexAttribArray(4);

        // bind circle radius on location 5
        glVertexAttribPointer(5, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, radius));
        glEnableVertexAttribArray(5);

        glBindBuffer(GL_ARRAY_BUFFER, 0); // Unbind the VBO
        glBindVertexArray(0); // Unbind the VAO
    }


    void addShape(const glm::vec2 &position, const glm::vec2 &scale, float rotation, uint32_t shape, const glm::vec4 &color, const Ref<Texture> &texture, const std::array<glm::vec2, 4> &texCoords) {
        int texId = 0;

        // Handle texture binding
        if (texture != nullptr) {
            if (std::find(textures.begin(), textures.end(), texture) == textures.end())
                textures.push_back(texture);

            for (int i = 0; i < textures.size(); i++) {
                if (textures[i] == texture) {
                    texId = i + 1;
                    break;
                }
            }
        }

        float radians = glm::radians(rotation);

        glm::mat2 rotationMatrix = glm::mat2(
                glm::cos(radians), -glm::sin(radians),
                glm::sin(radians), glm::cos(radians)
        );

        // Define vertices with bottom-left as the origin
        glm::vec2 verticesPos[4] = {
                rotationMatrix * glm::vec2(0.0f, 0.0f),                   // Bottom-left
                rotationMatrix * glm::vec2(scale.x, 0.0f),                // Bottom-right
                rotationMatrix * glm::vec2(scale.x, scale.y),             // Top-right
                rotationMatrix * glm::vec2(0.0f, scale.y)                 // Top-left
        };

        // Add transformed vertices to the vertex array with position, color, texture coordinates, texture ID, and shape ID
        for (int i = 0; i < 4; ++i) {
            vertices.emplace_back(
                    glm::vec3{position + verticesPos[i], zIndex}, color, texCoords[i], texId, shape
            );
        }

        // Add indices for the two triangles that form the quad
        indices.insert(indices.end(), {
                vertexIndex, vertexIndex + 1, vertexIndex + 2,  // First triangle (Bottom-left, Bottom-right, Top-right)
                vertexIndex, vertexIndex + 2, vertexIndex + 3   // Second triangle (Bottom-left, Top-right, Top-left)
        });

        vertexIndex += 4;

        if (vertexIndex >= maxBatchSize)
            full = true;
    }



    void render(int screenWidth, int screenHeight, Camera &camera) {

        //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        shader->bind();

        camera.applyViewport(screenWidth, screenHeight);
        shader->uploadMat4f("uWorldProjection", camera.getProjectionMatrix());
        shader->uploadMat4f("uView", camera.getViewMatrix());
        shader->uploadFloat("uTime", Time::getTime());

        for (int i = 0; i < textures.size(); i++) {
            glActiveTexture(GL_TEXTURE0 + i + 1);
            textures[i]->bind();
        }

        shader->uploadIntArray("uTextures", texSlots, 16);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(Vertex), vertices.data());

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, indices.size() * sizeof(uint32_t), indices.data());

        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

        for (int i = 0; i < textures.size(); i++) {
            textures[i]->unbind();
        }

        glBindVertexArray(0);
    }

    bool hasTextureRoom() {
        return textures.size() < 8;
    }

    bool hasTexture(const Ref<Texture> &texture) {
        return std::find(textures.begin(), textures.end(), texture) != textures.end();
    }

    bool isFull() const {
        return full;
    }

    int getZIndex() const {
        return zIndex;
    }

private:

    struct Vertex {
        glm::vec3 position;
        glm::vec4 color;
        glm::vec2 texCoords;
        float_t texID;
        float_t shape;
        float_t radius;
    };

    uint32_t maxBatchSize = 0;
    uint32_t zIndex{};
    bool full = false;

    GLuint VAO{}, VBO{}, EBO{};

    std::vector<Vertex> vertices;
    uint32_t vertexIndex = 0; // holds the drawing index in the element array
    std::vector<uint32_t> indices;

    Ref<Shader> shader;
    std::vector<Ref<Texture>> textures;
    int texSlots[16] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
};
