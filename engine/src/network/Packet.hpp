#pragma once

#include "core/Core.hpp"

class Packet {
public:
    enum Type {
        CONNECT,
        DISCONNECT,
        CHAT
    };
};

