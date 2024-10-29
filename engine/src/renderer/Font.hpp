#pragma once

#include "core/Core.hpp"

class Font {
public:
    virtual ~Font() = default;
    virtual void* getNativeResource() = 0;

    static Scope<Font> create(const std::string& filePath);
};

