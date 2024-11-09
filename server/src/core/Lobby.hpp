#pragma once

#include "CoreServer.hpp"
constexpr size_t MAX_PLAYERS = 4;

// Assuming a simple connection interface (this can be a class or struct you define elsewhere)
class Connection {
public:
    virtual void send_text(const std::string& message) = 0; // Pure virtual method for sending text
};

class Lobby {
public:
    Lobby(const std::string& id);
    ~Lobby();

    bool addPlayer(const std::string& playerId);
    bool removePlayer(const std::string& playerId);
    bool canAddPlayer() const;
    bool isEmpty() const;
    void broadcast(const std::string& message);
    void start();
    void stop();

private:
    void gameLoop();
    void updateGameState();
    void broadcastGameState();

    std::string id;
    bool running;
    std::thread gameThread;
    std::unordered_map<std::string, std::string> players; // Store only player IDs
    std::vector<std::shared_ptr<Connection>> connections; // A container for connections
    std::mutex mutex;
};
