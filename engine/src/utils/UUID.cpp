#include "UUID.hpp"

uint8_t UUID::getRandomByte() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<uint16_t> dis(0, 255);
    return static_cast<uint8_t>(dis(gen));
}

UUID UUID::randomUUID() {
    std::array<uint8_t, 16> bytes;
    for (auto &byte : bytes) {
        byte = getRandomByte();
    }

    bytes[6] = (bytes[6] & 0x0F) | 0x40;
    bytes[8] = (bytes[8] & 0x3F) | 0x80;

    return UUID(bytes);
}

uint64_t UUID::getMostSignificantBits() const {
    uint64_t mostSigBits = 0;
    for (int i = 0; i < 8; ++i) {
        mostSigBits = (mostSigBits << 8) | data[i];
    }
    return mostSigBits;
}

uint64_t UUID::getMaskedMostSignificantBits() {
    UUID uuid = UUID::randomUUID();
    uint64_t msb = uuid.getMostSignificantBits();
    return msb & std::numeric_limits<int64_t>::max();
}
