#include "Framebuffer.hpp"

Framebuffer::Framebuffer(int width, int height, bool useDepth) : width(width), height(height) {

    glGenFramebuffers(1, &framebufferObject);
    glBindFramebuffer(GL_FRAMEBUFFER, framebufferObject);

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

    if (useDepth) {
        glGenRenderbuffers(1, &renderBufferObject);
        glBindRenderbuffer(GL_RENDERBUFFER, renderBufferObject);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderBufferObject);
    }

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        AT_ERROR("Cannot create framebuffer.");
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

Framebuffer::~Framebuffer() {
    if (this->framebufferObject) {
        glDeleteFramebuffers(1, &framebufferObject);
    }
    if (texture) {
        glDeleteTextures(1, &texture);
    }
    if (renderBufferObject) {
        glDeleteRenderbuffers(1, &renderBufferObject);
    }
}

void Framebuffer::bind() const {
    glBindFramebuffer(GL_FRAMEBUFFER, framebufferObject);
    glViewport(0, 0, width, height); // Set viewport to framebuffer size
}

void Framebuffer::unbind() const {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::updateDimensions(uint32_t width, uint32_t height) {
    this->width = width;
    this->height = height;

    // Recreate the texture attachment
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

    // Recreate the renderbuffer if it exists
    if (renderBufferObject) {
        glBindRenderbuffer(GL_RENDERBUFFER, renderBufferObject);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    }

    // Unbind the framebuffer to avoid accidental rendering
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
