#pragma once

#include "core/Core.hpp"
#include <glad/glad.h>

class Framebuffer {
public:
    Framebuffer() = default;
    Framebuffer(int width, int height, bool useDepth = true);
    ~Framebuffer();

    void bind() const;

    void unbind() const;

    void updateDimensions(uint32_t width, uint32_t height);

private:
    GLuint framebufferObject;
    GLuint texture;
    GLuint renderBufferObject; // for depth
    uint32_t width, height;

};

