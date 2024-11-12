#pragma once

#include "core/Core.hpp"

using std::byte;

enum ServerPackets {
    welcome = 1
};

enum ClientPackets {
    welcomeReceived = 1
};

class Packet {
public:

private:
    std::vector<byte> buffer;
    byte* readableBuffer;
};

