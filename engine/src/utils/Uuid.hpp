#pragma once

#include <core/Core.hpp>

class Uuid {
private:
    std::array<uint8_t, 16> data;

    constexpr Uuid(const std::array<uint8_t, 16>& bytes) : data(bytes) {}

    static uint8_t getRandomByte();

public:
    static Uuid randomUUID();

    uint64_t getMostSignificantBits() const;

    uint64_t getLeastSignificantBits() const;

    static uint64_t getMaskedMostSignificantBits();

    std::string toString() const;
};

