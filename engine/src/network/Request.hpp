#pragma once
#include "Packet.hpp"

class Request {
public:
    virtual ~Request() = default;

    virtual void send(Packet packet) {}

    virtual void onIncomingRequest(Packet packet) {}

    virtual void onResponse() {}
};
