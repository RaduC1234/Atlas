#pragma once

#include <glm/glm.hpp>

class Color {
public:
    float r = 1.0f, g = 1.0f, b = 1.0f, a = 1.0f;

    constexpr Color() : r(0.0f), g(0.0f), b(0.0f), a(1.0f) {
    }

    constexpr Color(int r, int g, int b, int a = 255) : r(r / 255.0f), g(g / 255.0f), b(b / 255.0f), a(a / 255.0f) {
    }

    constexpr Color(float r, float g, float b, float a = 1.0f) : r(r), g(g), b(b), a(a) {
    }

    Color(const std::string &hex) {
        if (hex.length() == 8) {
            r = std::stoi(hex.substr(0, 2), nullptr, 16) / 255.0f;
            g = std::stoi(hex.substr(2, 2), nullptr, 16) / 255.0f;
            b = std::stoi(hex.substr(4, 2), nullptr, 16) / 255.0f;
            a = std::stoi(hex.substr(6, 2), nullptr, 16) / 255.0f;
        } else {
            AT_ERROR("Invalid hex color provided");
        }
    }

    // Cast to glm::vec4 - do not explicit it
    constexpr operator glm::vec4() const {
        return {r, g, b, a};
    }

    static constexpr Color white() { return Color(1.0f, 1.0f, 1.0f, 1.0f); }
    static constexpr Color black() { return Color(0.0f, 0.0f, 0.0f, 1.0f); }
    static constexpr Color gray() { return Color{75, 75, 75}; }
    static constexpr Color red() { return Color(1.0f, 0.0f, 0.0f, 1.0f); }
    static constexpr Color green() { return Color(0.0f, 1.0f, 0.0f, 1.0f); }
    static constexpr Color blue() { return Color(0.0f, 0.0f, 1.0f, 1.0f); }
    static constexpr Color transparent() { return Color(0.0f, 0.0f, 0.0f, 0.0f); }
};
