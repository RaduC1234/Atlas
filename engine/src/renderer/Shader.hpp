#pragma once

#include "core/Core.hpp"

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>


class Shader {
private:
    unsigned int shaderID = 0, vertexShaderID = 0, fragmentShaderID = 0;
    bool isBeingUsed = false;

    std::string filePath;

public:

    Shader() = default;

    explicit Shader(const std::string &filepath) : filePath(filepath) {
        try {
            std::string vertexSource, fragmentSource;

            std::ifstream file(filepath);
            if (!file.is_open()) {
                AT_WARN("Failed to open shader file: {0}", filepath);
                return;
            }

            std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
            file.close();

            size_t vertexPos = content.find("#type vertex");
            size_t fragmentPos = content.find("#type fragment");

            if (vertexPos == std::string::npos || fragmentPos == std::string::npos) {
                AT_WARN("Markers not found in the file: {0}", filepath);
                return;
            }

            size_t vertexEnd = content.find('\n', vertexPos) + 1;
            vertexSource = content.substr(vertexEnd, fragmentPos - vertexEnd);

            size_t fragmentEnd = content.find('\n', fragmentPos) + 1;
            fragmentSource = content.substr(fragmentEnd);

            loadAndCompile(vertexSource.c_str(), fragmentSource.c_str());

        } catch (const std::exception &e) {
            AT_ERROR(e.what());
            throw;
        }
    }

    Shader(const std::string &vertexShaderString, const std::string &fragmentShaderString) {
        loadAndCompile(vertexShaderString.c_str(), fragmentShaderString.c_str());
    }

    ~Shader() {
        remove();
    }

private:

    void loadAndCompile(const char *vertexSource, const char *fragmentSource) {

        // create a shader program
        shaderID = glCreateProgram();

        // vertex Shader
        this->vertexShaderID = glCreateShader(GL_VERTEX_SHADER);

        // load and compile the vertex shader
        glShaderSource(this->vertexShaderID, 1, &vertexSource, nullptr);
        glCompileShader(vertexShaderID);

        int success;
        glGetShaderiv(vertexShaderID, GL_COMPILE_STATUS, &success);

        // check for errors
        if (success == GL_FALSE) {
            int len;
            glGetShaderiv(vertexShaderID, GL_INFO_LOG_LENGTH, &len);
            std::string message(len, ' ');
            glGetShaderInfoLog(vertexShaderID, len, nullptr, &(message[0]));

            AT_ERROR("ERROR: Vertex shader compilation failed: {0}", message);
        }

        // fragment Shader
        this->fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

        // load and compile the fragment shader
        glShaderSource(this->fragmentShaderID, 1, &fragmentSource, nullptr);
        glCompileShader(fragmentShaderID);

        glGetShaderiv(fragmentShaderID, GL_COMPILE_STATUS, &success);

        // check for errors
        if (success == GL_FALSE) {
            int len;
            glGetShaderiv(fragmentShaderID, GL_INFO_LOG_LENGTH, &len);
            std::string message(len, ' ');
            glGetShaderInfoLog(fragmentShaderID, len, nullptr, &(message[0]));

            AT_ERROR("ERROR: Fragment shader compilation failed: {0}", message);
        }

        // attach shaders to the program
        glAttachShader(shaderID, vertexShaderID);
        glAttachShader(shaderID, fragmentShaderID);
        glLinkProgram(shaderID);

        // check for linking errors
        glGetProgramiv(shaderID, GL_LINK_STATUS, &success);

        if (success == GL_FALSE) {
            char log[1024];
            glGetProgramInfoLog(shaderID, 1024, nullptr, log);
            std::cout << "ERROR: Shader program linking failed: " << log << "\n";
        }

        // clean up shaders (no longer needed once linked)
        glDeleteShader(vertexShaderID);
        glDeleteShader(fragmentShaderID);
    }


public:

    const std::string &getFilePath() const {
        return filePath;
    }

    void bind() {
        if (!isBeingUsed) {
            glUseProgram(shaderID);
            isBeingUsed = true;
        }
    }

    void unbind() {
        glUseProgram(0);
        isBeingUsed = false;
    }

    void remove() {
        unbind();
        glDeleteProgram(shaderID);
    }

    void uploadMat4f(const std::string &varName, const glm::mat4 &mat) {
        int varLocation = glGetUniformLocation(shaderID, &(varName[0]));
        bind();
        glUniformMatrix4fv(varLocation, 1, GL_FALSE, glm::value_ptr(mat));
    }

    void uploadMat3f(const std::string &varName, const glm::mat3 &mat) {
        int varLocation = glGetUniformLocation(shaderID, &(varName[0]));
        bind();
        glUniformMatrix3fv(varLocation, 1, GL_FALSE, glm::value_ptr(mat));
    }

    void uploadVec4f(const std::string &varName, const glm::vec4 &vec) {
        int varLocation = glGetUniformLocation(shaderID, &(varName[0]));
        bind();
        glUniform4f(varLocation, vec.x, vec.y, vec.z, vec.w);
    }

    void uploadVec3f(const std::string &varName, const glm::vec3 &vec) {
        int varLocation = glGetUniformLocation(shaderID, &(varName[0]));
        bind();
        glUniform3f(varLocation, vec.x, vec.y, vec.z);
    }

    void uploadVec2f(const std::string &varName, const glm::vec2 &vec) {
        int varLocation = glGetUniformLocation(shaderID, &(varName[0]));
        bind();
        glUniform2f(varLocation, vec.x, vec.y);
    }


    void uploadFloat(const std::string &varName, float value) {
        int varLocation = glGetUniformLocation(shaderID, &(varName[0]));
        bind();
        glUniform1f(varLocation, value);
    }

    void uploadInt(const std::string &varName, int value) {
        int varLocation = glGetUniformLocation(shaderID, &(varName[0]));
        bind();
        glUniform1i(varLocation, value);
    }

    void uploadTexture(const std::string &varName, int slot) {
        uploadInt(varName, slot);
    }

    void uploadIntArray(std::string varName, int array[], int size) {
        int varLocation = glGetUniformLocation(shaderID, &(varName[0]));
        bind();
        glUniform1iv(varLocation, size, array);
    }
};
