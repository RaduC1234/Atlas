#pragma once

#include <Atlas.hpp>

class Framebuffer {
public:
    Framebuffer() = default;

    Framebuffer(int width, int height, bool useDepth = true);

    ~Framebuffer();

    void bind() const;

    void unbind() const;

    void updateDimensions(uint32_t width, uint32_t height);

private:
    uint32_t framebufferObject;
    uint32_t texture;
    uint32_t renderBufferObject; // for depth
    uint32_t width, height;
};
