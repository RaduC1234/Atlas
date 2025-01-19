#pragma once

#include <Atlas.hpp>

class Framebuffer {
public:
    Framebuffer() = default;

    Framebuffer(int width, int height);

    ~Framebuffer();

    void bind() const;

    void unbind() const;

    void updateDimensions(uint32_t width, uint32_t height);

    uint32_t getTextureID() const;

    uint32_t getWidth() const {
        return width;
    }

    uint32_t getHeight() const {
        return height;
    }

private:
    uint32_t framebufferObject;
    uint32_t texture;
    uint32_t width, height;
};
