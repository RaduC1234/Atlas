#pragma once

#include "core/Core.hpp"

class Font {
    virtual ~Font();

    virtual std::string getTTFPath();
};

