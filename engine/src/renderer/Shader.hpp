#pragma once

#include "core/Core.hpp"
#include <glm/glm.hpp>


class Shader {
public:

    Shader() = default;

    explicit Shader(const std::string &filepath);

    Shader(const std::string &vertexShaderString, const std::string &fragmentShaderString);

    ~Shader();

private:
    void loadAndCompile(const char *vertexSource, const char *fragmentSource);

public:

    const std::string &getFilePath() const {
        return filePath;
    }

    void bind();

    void unbind();

    void remove();

    void uploadMat4f(const std::string &varName, const glm::mat4 &mat);

    void uploadMat3f(const std::string &varName, const glm::mat3 &mat);

    void uploadVec4f(const std::string &varName, const glm::vec4 &vec);

    void uploadVec3f(const std::string &varName, const glm::vec3 &vec);

    void uploadVec2f(const std::string &varName, const glm::vec2 &vec);

    void uploadFloat(const std::string &varName, float value);

    void uploadInt(const std::string &varName, int value);

    void uploadTexture(const std::string &varName, int slot);

    void uploadIntArray(std::string varName, int array[], int size);

private:
    unsigned int shaderID = 0, vertexShaderID = 0, fragmentShaderID = 0;
    bool isBeingUsed = false;

    std::string filePath;

};
