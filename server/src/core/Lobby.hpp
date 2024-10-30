#pragma once
#include <SFML/Network.hpp>
#include <vector>
#include <mutex>
#include <thread>
#include <algorithm>
class Lobby {
private:
    int lobbyId;
    std::vector<sf::TcpSocket*> players;
    std::thread lobbyThread;
    mutable std::mutex lobbyMutex;
    bool isRunning = true;
    static const int MAX_PLAYERS = 10;

    void lobbyLoop();

public:
    explicit Lobby(int id);
    ~Lobby();

    bool addPlayer(sf::TcpSocket* player);
    void removePlayer(sf::TcpSocket* player);
    int getPlayerCount() const;
    int getLobbyId() const;

    void notifyShutdown();
};