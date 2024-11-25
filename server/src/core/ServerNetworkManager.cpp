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

    CROW_WEBSOCKET_ROUTE(serverApp, "/")
            .onopen([](crow::websocket::connection &conn) {
                std::lock_guard lock(classMutex);
                const Client newClient(&conn);
                users.insert(newClient);
                AT_INFO("New client connected with remote ip: {0}", conn.get_remote_ip());
            })
            .onclose([](crow::websocket::connection &conn, const std::string &reason) {
                std::lock_guard lock(classMutex);
                const auto it = std::ranges::find_if(users, [&conn](const Client &client) {
                    return client.getConnection() == &conn;
                });

                AT_INFO("Client {0} disconnected. Reason {1}",
                        ((*it).getConnection() == nullptr ? "" : "[" + (*it).getConnection()->get_remote_ip() + "]"),
                        reason);


                if (it != users.end()) {
                    users.erase(it);
                }
            })
            .onerror([&](crow::websocket::connection &conn, const std::string &error_message) {
                std::lock_guard lock(classMutex);
                const auto it = std::ranges::find_if(users, [&conn](const Client &client) {
                    return client.getConnection() == &conn;
                });

                AT_INFO("Client {0} disconnected. Reason {1}",
                        ((*it).getConnection() == nullptr ? "" : "[" + (*it).getConnection()->get_remote_ip() + "]"),
                        error_message);


                if (it != users.end()) {
                    users.erase(it);
                }
            })
            .onmessage([](crow::websocket::connection &conn, const std::string &data, bool is_binary) {
                if (is_binary) {
                    conn.send_binary(data);
                } else {
                    conn.send_text(data);
                }
            });

    serverApp.port(port).run();
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
