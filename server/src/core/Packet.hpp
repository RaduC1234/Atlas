#pragma once

#include "core/Client.hpp"
#include "core/Core.hpp"

enum RequestType {
    GET,
    POST
};

struct Packet {
    crow::request &nativeRequest;
    long long UUID;
    Client client;
    RequestType type;
    JsonData payload;
};
