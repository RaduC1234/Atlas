#pragma once

#include "core/Core.hpp"

#include <SFML/Network.hpp>
class Lobby {
public:
    explicit Lobby(int id);
    ~Lobby();

    bool addPlayer(sf::TcpSocket* player);
    void removePlayer(sf::TcpSocket* player);
    int getPlayerCount() const;
    int getLobbyId() const;
    //void sendLevel();
    //void sendPacket(Packet packet);

    void notifyShutdown();

private:
    int lobbyId;
    std::vector<sf::TcpSocket*> players; // clasa separatata cu player PlayerConnection, Player din data si un enum CONNECTED, DISCONECTED AND WAITING
    std::thread lobbyThread;
    mutable std::mutex lobbyMutex;
    bool isRunning = true;
    static const int MAX_PLAYERS = 4;

    void lobbyLoop();

};

/**
* Packet
*/
