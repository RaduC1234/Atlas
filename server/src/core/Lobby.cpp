#include "Lobby.hpp"

Lobby::Lobby(const std::string& id) : id(id), running(false) {}

Lobby::~Lobby() {
    stop();
}

bool Lobby::addPlayer(const std::string& playerId) {
    std::lock_guard<std::mutex> lock(mutex);
    if (players.size() < MAX_PLAYERS) {
        players[playerId] = "default";
        std::cout << "Player " << playerId << " added to lobby " << id << std::endl;
        return true;
    }
    return false;
}

bool Lobby::removePlayer(const std::string& playerId) {
    std::lock_guard<std::mutex> lock(mutex);
    auto it = players.find(playerId);
    if (it != players.end()) {
        players.erase(it);
        std::cout << "Player " << playerId << " removed from lobby " << id << std::endl;
        return true;
    }
    return false;
}

bool Lobby::canAddPlayer() const {
    return players.size() < MAX_PLAYERS;
}

bool Lobby::isEmpty() const {
    return players.empty();
}

void Lobby::broadcast(const std::string& message) {
    std::lock_guard<std::mutex> lock(mutex);
    for (auto& conn : connections) {
        conn->send_text(message);
    }
}

void Lobby::start() {
    if (running) return;
    running = true;
    gameThread = std::thread(&Lobby::gameLoop, this);
    std::cout << "Lobby " << id << " has started" << std::endl;
}

void Lobby::stop() {
    if (!running) return;
    running = false;
    if (gameThread.joinable()) {
        gameThread.join();
        std::cout << "Lobby " << id << " has stopped" << std::endl;
    }
}

void Lobby::gameLoop() {
    while (running) {
        updateGameState();
        broadcastGameState();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

void Lobby::updateGameState() {
    std::cout << "Updating game state in lobby " << id << std::endl;
}

void Lobby::broadcastGameState() {
    std::cout << "Broadcasting game state in lobby " << id << std::endl;
}
