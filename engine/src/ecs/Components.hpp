// Components.hpp
#pragma once

#include <string>

struct Position {
    float x;
    float y;
};

struct Velocity {
    float dx;
    float dy;
};

struct Health {
    int hp;
};

struct Sprite {
    std::string texturePath;
    float width;
    float height;
    float scale;

    explicit Sprite(std::string path = "", float w = 32.0f, float h = 32.0f, float s = 1.0f)
        : texturePath(std::move(path)), width(w), height(h), scale(s) {}
};
