#pragma once
#include "CoreServer.hpp"
class ServerNetworkManager {
public:
    ServerNetworkManager(const std::string& ip, uint16_t port, 
                        CommandHandler* cmdHandler, 
                        RequestHandler* reqHandler);
    ~ServerNetworkManager();

    void start();
    void stop();

    std::string createLobby();
    void deleteLobby(const std::string& lobbyId);
    Lobby* getLobby(const std::string& lobbyId);

private:
    std::string ip;
    uint16_t port;
    bool running;
    
    crow::SimpleApp app;
    std::thread serverThread;
    
    CommandHandler* commandHandler;
    RequestHandler* requestHandler;
    
    std::unordered_map<std::string, std::unique_ptr<Lobby>> lobbies;
    std::mutex lobbiesMutex;

    void setupRoutes();
    void setupWebSocket();
    void handleWebSocket(crow::websocket::connection& conn);
    std::string generateLobbyId();

    // WebSocket message handlers
    void handlePlayerJoin(const std::string& lobbyId, const std::string& playerId);
    void handlePlayerLeave(const std::string& lobbyId, const std::string& playerId);
    void broadcastToLobby(const std::string& lobbyId, const std::string& message);
};