#pragma once
#include "CoreServer.hpp"
class Server {
public:
    Server(const std::string& ip, uint16_t port);
    ~Server();

    void start();
    void stop();
    bool isRunning() const;

private:
    std::string ip;
    uint16_t port;
    bool running;

    std::unique_ptr<ServerNetworkManager> networkManager;
    std::unique_ptr<CommandHandler> commandHandler;
    std::unique_ptr<RequestHandler> requestHandler;

    void initializeServer();
    void setupSignalHandlers();
};