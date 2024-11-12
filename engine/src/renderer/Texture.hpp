#pragma once

#include <stb_image.h>
#include <glad/glad.h>

#include "core/Core.hpp"

class Texture {
public:
    explicit Texture(const std::string& filePath) : filePath(filePath) {
        generateAndLoad(filePath.c_str());
    }

    Texture(uint32_t texID) : textureID(texID) {}

   /* ~Texture() {
        glDeleteTextures(1, &textureID);
    }*/

private:
    void generateAndLoad(const char *filePth) {

        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);

        // Set the texture parameters
        // Repeat the image in both directions
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        // When stretching the image, pixelate
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        // When shrinking an image, pixelate
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        auto data = stbi_load(filePth, &width, &height, &channel, 0);

        if (data) {
            GLenum format;
            if (channel == 1)
                format = GL_RED;
            else if (channel == 3)
                format = GL_RGB;
            else if (channel == 4)
                format = GL_RGBA;

            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        } else {
            std::stringstream ss;
            ss << "Error loading texture: " << filePth;
            AT_ERROR(ss.str());
        }


        stbi_image_free(data);
    }

public:
    void bind() const {
        glBindTexture(GL_TEXTURE_2D, textureID);
    }

    void unbind() {
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    const std::string &getFilePath() const {
        return filePath;
    }

    int getWidth() const {
        return width;
    }

    int getHeight() const {
        return height;
    }

    GLuint getTextureId() const {
        return textureID;
    }

private:
    GLuint textureID;
    int width, height, channel;

    std::string filePath;

};

