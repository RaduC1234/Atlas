#pragma once

#include <crow/http_response.h>

#include "Packet.hpp"

class ServerRouteHandler {
public:
    virtual ~ServerRouteHandler() = default;

    virtual crow::response handle(const Packet &request) = 0;
};
