
#pragma once
//#include <sstream>
#include <iostream>
#include <chrono>
#include <thread>
#include <SFML/Network.hpp>
#include <map>
#include <memory>
#include <atomic>
#include "Lobby.hpp"
#include "Atlas.hpp"



class ServerNetworkingManager {
private:
    static const int MAX_CLIENTS = 10;
    std::map<int, std::unique_ptr<Lobby>> lobbies;
    std::vector<sf::TcpSocket*> clients;
    std::mutex serverMutex;
    int nextLobbyId = 1;
    std::atomic<bool>& serverRunning;

    static bool verifyDLL(const std::vector<char>& buffer); // nup
    void removeClient(sf::TcpSocket* client);
    void handleLobbyRequests(sf::TcpSocket* client);
    void createLobby(sf::TcpSocket* client);
    void joinLobby(sf::TcpSocket* client, sf::Packet& packet);
    void showHelp(); // move in command
public:
    ServerNetworkingManager(std::atomic<bool>& running);
    void handleClient(sf::TcpSocket* client);
    void handleCommand(const std::string& command); // in command
    bool addClient(sf::TcpSocket* client);
};
