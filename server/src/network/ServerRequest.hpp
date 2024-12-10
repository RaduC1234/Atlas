#pragma once

#include <crow/http_response.h>

#include "Packet.hpp"

class ServerRequest {
public:
    virtual ~ServerRequest() = default;

    virtual crow::response handle(const Packet &request) = 0;
};
