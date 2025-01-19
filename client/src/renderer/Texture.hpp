#pragma once

#include <Atlas.hpp>

class Texture {
public:
    Texture(const std::string& filePath);

    Texture(uint32_t texID) : textureID(texID) {} // to be used only in framebuffer

   /* ~Texture() {
        glDeleteTextures(1, &textureID);
    }*/

private:
    void generateAndLoad(const char *filePth);

public:
    void bind() const;

    void unbind();

    const std::string &getFilePath() const {
        return filePath;
    }

    int getWidth() const {
        return width;
    }

    int getHeight() const {
        return height;
    }

    uint32_t getTextureId() const {
        return textureID;
    }

private:
    uint32_t textureID;
    int width, height, channel;

    std::string filePath;

};



