#pragma once

#include <Atlas.hpp>
#include "Client.hpp"
#include "Request.hpp"

#include <crow/app.h>

using TickAction = std::function<void()>;

class ServerNetworkManager {
public:
    static void addRequestHandler(const std::string &name, Scope<Request> handler);

    static void start(uint32_t port);

    static void tick();

    static void shutdown();

private:
    static std::mutex classMutex;
    static crow::SimpleApp serverApp;
    static std::unordered_map<std::string, Scope<Request>> requests;
    static std::set<Client> users;
    static std::queue<TickAction> actions;
};
