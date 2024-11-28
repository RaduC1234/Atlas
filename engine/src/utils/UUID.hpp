#pragma once

#include <array>
#include <cstdint>
#include <random>
#include <string>
#include <sstream>
#include <iomanip>
#include <limits>

class UUID {
private:
    std::array<uint8_t, 16> data;

    UUID(const std::array<uint8_t, 16>& bytes) : data(bytes) {}

    static uint8_t getRandomByte();

public:
    static UUID randomUUID();

    uint64_t getMostSignificantBits() const;

    uint64_t getLeastSignificantBits() const {
        uint64_t leastSigBits = 0;
        for (int i = 8; i < 16; ++i) {
            leastSigBits = (leastSigBits << 8) | data[i];
        }
        return leastSigBits;
    }

    static uint64_t getMaskedMostSignificantBits();

    std::string toString() const {
        std::ostringstream oss;
        for (size_t i = 0; i < data.size(); ++i) {
            oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(data[i]);
            if (i == 3 || i == 5 || i == 7 || i == 9) {
                oss << '-';
            }
        }
        return oss.str();
    }
};
