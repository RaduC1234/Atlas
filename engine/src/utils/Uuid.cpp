#include "Uuid.hpp"

uint8_t Uuid::getRandomByte() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<uint16_t> dis(0, 255);
    return static_cast<uint8_t>(dis(gen));
}

Uuid Uuid::randomUUID() {
    std::array<uint8_t, 16> bytes;
    for (auto &byte : bytes) {
        byte = getRandomByte();
    }

    bytes[6] = (bytes[6] & 0x0F) | 0x40;
    bytes[8] = (bytes[8] & 0x3F) | 0x80;

    return Uuid(bytes);
}

uint64_t Uuid::getMostSignificantBits() const {
    uint64_t mostSigBits = 0;
    for (int i = 0; i < 8; ++i) {
        mostSigBits = (mostSigBits << 8) | data[i];
    }
    return mostSigBits;
}

uint64_t Uuid::getLeastSignificantBits() const {
    uint64_t leastSigBits = 0;
    for (int i = 8; i < 16; ++i) {
        leastSigBits = (leastSigBits << 8) | data[i];
    }
    return leastSigBits;
}

uint64_t Uuid::getMaskedMostSignificantBits() {
    Uuid uuid = Uuid::randomUUID();
    uint64_t msb = uuid.getMostSignificantBits();
    return msb & std::numeric_limits<int64_t>::max();
}

std::string Uuid::toString() const {
    std::ostringstream oss;
    for (size_t i = 0; i < data.size(); ++i) {
        oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(data[i]);
        if (i == 3 || i == 5 || i == 7 || i == 9) {
            oss << '-';
        }
    }
    return oss.str();
}

