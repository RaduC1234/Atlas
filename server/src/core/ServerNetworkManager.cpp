#include "ServerNetworkManager.hpp"

using json = nlohmann::json;

ServerNetworkManager::ServerNetworkManager(
    const std::string& ip, uint16_t port,
    CommandHandler* cmdHandler,
    RequestHandler* reqHandler)
    : ip(ip)
    , port(port)
    , running(false)
    , commandHandler(cmdHandler)
    , requestHandler(reqHandler) {
    setupRoutes();
    setupWebSocket();
}

ServerNetworkManager::~ServerNetworkManager() {
    if (running) {
        stop();
    }
}

void ServerNetworkManager::setupRoutes() {
    CROW_ROUTE(app, "/api/lobby/create")
    .methods("POST"_method)
    ([this](const crow::request& req) {
        try {
            auto params = json::parse(req.body);
            auto response = requestHandler->handleCreateLobbyRequest(params);
            return crow::response(200, response.dump());
        } catch (const std::exception& e) {
            return crow::response(400, "Invalid request: " + std::string(e.what()));
        }
    });

    CROW_ROUTE(app, "/api/lobby/join")
    .methods("POST"_method)
    ([this](const crow::request& req) {
        try {
            auto params = json::parse(req.body);
            auto lobbyId = params["lobbyId"].get<std::string>();
            auto playerId = params["playerId"].get<std::string>();

            auto lobby = getLobby(lobbyId);
            if (!lobby) {
                return crow::response(404, "Lobby not found");
            }

            if (!lobby->canAddPlayer()) {
                return crow::response(403, "Lobby is full");
            }

            handlePlayerJoin(lobbyId, playerId);
            return crow::response(200);
        } catch (const std::exception& e) {
            return crow::response(400, "Invalid request: " + std::string(e.what()));
        }
    });
}

void ServerNetworkManager::setupWebSocket() {
    CROW_WEBSOCKET_ROUTE(app, "/ws")
    .onopen([&](crow::websocket::connection& conn) {
        std::cout << "WebSocket connection opened" << std::endl;
    })
    .onclose([&](crow::websocket::connection& conn, const std::string& reason) {
        std::cout << "WebSocket connection closed: " << reason << std::endl;
    })
    .onmessage([&](crow::websocket::connection& conn, const std::string& data, bool is_binary) {
        if (!is_binary) {
            try {
                auto j = json::parse(data);
                std::string command = j["command"];

                if (command == "join_lobby") {
                    handlePlayerJoin(j["lobbyId"], j["playerId"]);
                } else if (command == "leave_lobby") {
                    handlePlayerLeave(j["lobbyId"], j["playerId"]);
                } else {
                    commandHandler->handleCommand(j);
                }
            } catch (const std::exception& e) {
                std::cerr << "Error handling WebSocket message: " << e.what() << std::endl;
                conn.send_text("Error: Invalid message format");
            }
        }
    });
}

void ServerNetworkManager::start() {
    if (running) {
        return;
    }

    running = true;
    serverThread = std::thread([this]() {
        try {
            app.bindaddr(ip)
               .port(port)
               .multithreaded()
               .run();
        } catch (const std::exception& e) {
            std::cerr << "Error starting server: " << e.what() << std::endl;
            running = false;
        }
    });
}

void ServerNetworkManager::stop() {
    if (!running) {
        return;
    }

    std::cout << "Stopping server..." << std::endl;
    app.stop();

    if (serverThread.joinable()) {
        serverThread.join();
    }

    // Cleanup all lobbies
    std::lock_guard<std::mutex> lock(lobbiesMutex);
    lobbies.clear();
    running = false;
}

std::string ServerNetworkManager::createLobby() {
    std::lock_guard<std::mutex> lock(lobbiesMutex);
    std::string lobbyId = generateLobbyId();

    auto lobby = std::make_unique<Lobby>(lobbyId);
    lobbies[lobbyId] = std::move(lobby);

    return lobbyId;
}

void ServerNetworkManager::deleteLobby(const std::string& lobbyId) {
    std::lock_guard<std::mutex> lock(lobbiesMutex);
    lobbies.erase(lobbyId);
}

Lobby* ServerNetworkManager::getLobby(const std::string& lobbyId) {
    std::lock_guard<std::mutex> lock(lobbiesMutex);
    auto it = lobbies.find(lobbyId);
    return it != lobbies.end() ? it->second.get() : nullptr;
}

std::string ServerNetworkManager::generateLobbyId() {
    static const char chars[] = "0123456789ABCDEF";
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> dis(0, sizeof(chars) - 2);

    std::string id;
    for (int i = 0; i < 6; ++i) {
        id += chars[dis(gen)];
    }
    return id;
}

void ServerNetworkManager::handlePlayerJoin(const std::string& lobbyId, const std::string& playerId) {
    auto lobby = getLobby(lobbyId);
    if (lobby) {
        lobby->addPlayer(playerId);
        broadcastToLobby(lobbyId, json{
            {"type", "player_joined"},
            {"playerId", playerId}
        }.dump());
    }
}

void ServerNetworkManager::handlePlayerLeave(const std::string& lobbyId, const std::string& playerId) {
    auto lobby = getLobby(lobbyId);
    if (lobby) {
        lobby->removePlayer(playerId);
        broadcastToLobby(lobbyId, json{
            {"type", "player_left"},
            {"playerId", playerId}
        }.dump());

        if (lobby->isEmpty()) {
            deleteLobby(lobbyId);
        }
    }
}

void ServerNetworkManager::broadcastToLobby(const std::string& lobbyId, const std::string& message) {
    auto lobby = getLobby(lobbyId);
    if (lobby) {
        lobby->broadcast(message);
    }
}
