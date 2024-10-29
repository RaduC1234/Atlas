#pragma once

#include "core/Core.hpp"

class Texture {
public:
    virtual ~Texture() = default;

    virtual uint32_t getWidth() = 0;
    virtual uint32_t getHeight() = 0;

    virtual void *getNativeTexture() = 0;

private:
    static Scope<Texture> create(const std::string &filepath);

    friend class ResourceBundle;
};

