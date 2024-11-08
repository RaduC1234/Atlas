#include "Lobby.hpp"


Lobby::Lobby(int id) : lobbyId(id), lobbyThread(&Lobby::lobbyLoop, this) {
}

Lobby::~Lobby() {
    isRunning = false;
    if (lobbyThread.joinable()) {
        lobbyThread.join();
    }

    std::lock_guard<std::mutex> lock(lobbyMutex);
    players.clear();
}

bool Lobby::addPlayer(sf::TcpSocket* player) {
    std::lock_guard<std::mutex> lock(lobbyMutex);
    if (players.size() >= MAX_PLAYERS) {
        return false;
    }
    players.push_back(player);
    return true;
}

void Lobby::removePlayer(sf::TcpSocket* player) {
    std::lock_guard<std::mutex> lock(lobbyMutex);
    auto it = std::find(players.begin(), players.end(), player);
    if (it != players.end()) {
        players.erase(it);
    }
}

int Lobby::getPlayerCount() const {
    std::lock_guard<std::mutex> lock(lobbyMutex);
    return static_cast<int>(players.size());
}

int Lobby::getLobbyId() const {
    return lobbyId;
}

void Lobby::lobbyLoop() {
    while (isRunning) {
        // Main lobby logic here
        std::this_thread::sleep_for(std::chrono::milliseconds(16)); // ~60 fps
    }
}

void Lobby::notifyShutdown() {
    std::lock_guard<std::mutex> lock(lobbyMutex);
    sf::Packet shutdownPacket;
    shutdownPacket << "LOBBY_SHUTDOWN" << "Server is shutting down";

    for (auto player : players) {
        try {
            player->send(shutdownPacket);
        }
        catch (...) {
            // Ignore errors during shutdown
        }
    }
}