#include "ServerNetworkManager.hpp"
#include <queue>

#include "ServerNetworkManager.hpp"
#include <queue>

#include "CrowLogger.hpp"

// Define static members
std::mutex ServerNetworkManager::classMutex;
crow::SimpleApp ServerNetworkManager::serverApp;
std::unordered_map<std::string, Scope<Request> > ServerNetworkManager::requests;
std::set<Client> ServerNetworkManager::users;
std::queue<TickAction> ServerNetworkManager::actions;

void ServerNetworkManager::addRequestHandler(const std::string &name, Scope<Request> handler) {
    std::lock_guard<std::mutex> lock(classMutex);
    requests[name] = std::move(handler);
}

void ServerNetworkManager::start(uint32_t port) {
    const auto logger = CreateRef<CrowLogger>();
    crow::logger::setHandler(logger.get());

    CROW_ROUTE(serverApp, "/")([&](const crow::request& request) {
        return "<h1>Hello World</h1>";
    });


    serverApp.multithreaded().port(8080).run();
}


void ServerNetworkManager::tick() {
    std::queue<TickAction> pendingActions; {
        std::lock_guard<std::mutex> lock(classMutex);
        std::swap(actions, pendingActions);
    }

    while (!pendingActions.empty()) {
        auto action = pendingActions.front();
        pendingActions.pop();
        action();
    }
}

void ServerNetworkManager::shutdown() {
    std::lock_guard<std::mutex> lock(classMutex);

    // Notify all clients about server shutdown
    for (auto &client: users) {
        client.getConnection()->send_text("Server is shutting down.");
    }
    users.clear();
    AT_INFO("Server is shutting down.");
}
