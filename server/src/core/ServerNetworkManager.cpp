#include "ServerNetworkManager.hpp"


static constexpr size_t BUFFER_SIZE = 8192;
static constexpr size_t MAX_DLL_SIZE = 10 * 1024 * 1024;  // 10MB limit
static constexpr auto CLIENT_TIMEOUT = std::chrono::seconds(30);

ServerNetworkingManager::ServerNetworkingManager(std::atomic<bool>& running)
    : serverRunning(running) {}

bool ServerNetworkingManager::verifyDLL(const std::vector<char>& buffer) {
    if (buffer.size() < 64) {  // Minimum size for PE header
        return false;
    }

    // Check MZ signature
    if (buffer[0] != 'M' || buffer[1] != 'Z') {
        return false;
    }

    // Get PE header offset (at 0x3C)
    uint32_t peOffset = *reinterpret_cast<const uint32_t*>(&buffer[0x3C]);

    // Verify PE offset is within bounds
    if (peOffset >= buffer.size() - 4) {
        return false;
    }

    // Check PE signature ("PE\0\0")
    return buffer[peOffset] == 'P' &&
           buffer[peOffset + 1] == 'E' &&
           buffer[peOffset + 2] == 0 &&
           buffer[peOffset + 3] == 0;
}

void ServerNetworkingManager::handleClient(sf::TcpSocket* client) {
    if (!client) {
        AT_ERROR("Null client pointer received");
        return;
    }

    try {
        // Set timeout
        client->setBlocking(false);

        std::vector<char> buffer;
        buffer.reserve(BUFFER_SIZE);
        size_t totalReceived = 0;
        auto startTime = std::chrono::steady_clock::now();

        while (totalReceived < MAX_DLL_SIZE) {
            std::vector<char> chunk(BUFFER_SIZE);
            size_t received;

            auto status = client->receive(chunk.data(), chunk.size(), received);

            if (status == sf::Socket::Done) {
                buffer.insert(buffer.end(), chunk.begin(), chunk.begin() + received);
                totalReceived += received;
                break;  // Added break to exit after successful receive
            }
            else if (status == sf::Socket::NotReady) {
                // Check timeout
                if (std::chrono::steady_clock::now() - startTime > CLIENT_TIMEOUT) {
                    throw std::runtime_error("Client timeout");
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
                continue;
            }
            else {
                throw std::runtime_error("Socket error during receive");
            }
        }

        if (totalReceived >= MAX_DLL_SIZE) {
            throw std::runtime_error("DLL size exceeds maximum allowed");
        }

        bool isDLL = verifyDLL(buffer);

        sf::Packet responsePacket;
        responsePacket << (isDLL ? "DLL_VERIFIED" : "INVALID_DLL");

        if (client->send(responsePacket) != sf::Socket::Done) {
            throw std::runtime_error("Failed to send response");
        }

        AT_INFO("Data received from client: {} bytes, {}",
               totalReceived, isDLL ? "Valid DLL" : "Invalid DLL");

        if (isDLL) {
            handleLobbyRequests(client);
        }
    }
    catch (const std::exception& e) {
        AT_ERROR("Error handling client: {}", e.what());

        try {
            sf::Packet errorPacket;
            errorPacket << "ERROR" << e.what();
            client->send(errorPacket);
        } catch (...) {
            // Ignore send errors during error handling
        }
    }

    removeClient(client);
    delete client;
}

void ServerNetworkingManager::removeClient(sf::TcpSocket* client) {
    std::lock_guard<std::mutex> lock(serverMutex);
    auto it = std::find(clients.begin(), clients.end(), client);
    if (it != clients.end()) {
        clients.erase(it);
    }

    for (auto& lobby : lobbies) {
        lobby.second->removePlayer(client);
    }
}

bool ServerNetworkingManager::addClient(sf::TcpSocket* client) {
    std::lock_guard<std::mutex> lock(serverMutex);
    if (clients.size() >= MAX_CLIENTS) {
        return false;
    }
    clients.push_back(client);
    return true;
}

void ServerNetworkingManager::handleLobbyRequests(sf::TcpSocket* client) {
    while (serverRunning) {
        try {
            sf::Packet packet;
            auto status = client->receive(packet);

            if (status == sf::Socket::Disconnected) {
                AT_INFO("Client disconnected");
                break;
            }
            else if (status != sf::Socket::Done) {
                throw std::runtime_error("Error receiving lobby request");
            }

            std::string command;
            if (!(packet >> command)) {
                throw std::runtime_error("Invalid packet format");
            }

            if (command == "CREATE_LOBBY") {
                createLobby(client);
            }
            else if (command == "JOIN_LOBBY") {
                joinLobby(client, packet);
            }
            else {
                throw std::runtime_error("Unknown lobby command: " + command);
            }
        }
        catch (const std::exception& e) {
            AT_ERROR("Lobby request error: {}", e.what());

            try {
                sf::Packet errorPacket;
                errorPacket << "ERROR" << e.what();
                client->send(errorPacket);
            } catch (...) {
                break;
            }
        }
    }
}

void ServerNetworkingManager::handleCommand(const std::string& command) {
    if (command == "status") {
        AT_INFO("Server is running and accepting clients.");
    } else if (command == "shutdown") {
        AT_INFO("Shutting down the server.");
        serverRunning = false;
    } else if (command == "list_clients") {
        std::lock_guard<std::mutex> lock(serverMutex);
        AT_INFO("Connected clients: {}/{}", clients.size(), MAX_CLIENTS);
        AT_INFO("Active lobbies: {}", lobbies.size());
        for (const auto& lobby : lobbies) {
            AT_INFO("Lobby {}: {}/4 players", lobby.first, lobby.second->getPlayerCount());
        }
    } else if (command == "help") {
        showHelp();
    } else {
        AT_WARN("Unknown command: {}", command);
    }
}

// Helper functions
void ServerNetworkingManager::createLobby(sf::TcpSocket* client) {
    std::lock_guard<std::mutex> lock(serverMutex);
    int lobbyId = nextLobbyId++;
    lobbies[lobbyId] = std::make_unique<Lobby>(lobbyId);

    if (!lobbies[lobbyId]->addPlayer(client)) {
        lobbies.erase(lobbyId);
        throw std::runtime_error("Failed to add player to new lobby");
    }

    sf::Packet response;
    response << "LOBBY_CREATED" << lobbyId;
    if (client->send(response) != sf::Socket::Done) {
        throw std::runtime_error("Failed to send lobby creation response");
    }

    AT_INFO("Created lobby {}", lobbyId);
}

void ServerNetworkingManager::joinLobby(sf::TcpSocket* client, sf::Packet& packet) {
    int lobbyId;
    if (!(packet >> lobbyId)) {
        throw std::runtime_error("Invalid lobby ID format");
    }

    std::lock_guard<std::mutex> lock(serverMutex);
    auto it = lobbies.find(lobbyId);

    if (it == lobbies.end()) {
        throw std::runtime_error("Lobby not found: " + std::to_string(lobbyId));
    }

    if (!it->second->addPlayer(client)) {
        throw std::runtime_error("Lobby is full");
    }

    sf::Packet response;
    response << "JOINED_LOBBY" << lobbyId;
    if (client->send(response) != sf::Socket::Done) {
        it->second->removePlayer(client);
        throw std::runtime_error("Failed to send join response");
    }

    AT_INFO("Client joined lobby {}", lobbyId);
}

void ServerNetworkingManager::showHelp() {
    std::cout << "Available commands:\n";
    std::cout << " - help: Show this help message\n";
    std::cout << " - status: Show server status\n";
    std::cout << " - shutdown: Shutdown the server\n";
    std::cout << " - list_clients: Show the number of clients and active lobbies\n";
}