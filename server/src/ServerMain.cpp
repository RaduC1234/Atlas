#include "Atlas.hpp"
#include "ServerNetworkManager.hpp"
#include <SFML/Network.hpp>


//TODO:Integrate PlayerService  in the server
//TODO:Implement the DatabaseManager for the player only
//TODO:Make the connection between the log and ServerNetwork , log for the server only ,
//TODO:#optional make it connect to a discord server and send the massages of the
//TODO:server status ticks etc on the chat so that its easy to see and even on help commands
std::atomic<bool> serverRunning{true};

void commandListener(ServerNetworkingManager& manager) {
    std::string command;
    while (serverRunning) {
        std::getline(std::cin, command);
        manager.handleCommand(command);
    }
}

int main(int argc, char** argv) {
    Log::init();
    AT_INFO("Server started");

    sf::TcpListener listener;
    if (listener.listen(53000) != sf::Socket::Done) {
        AT_ERROR("Error binding to port 53000");
        return -1;
    }

    AT_INFO("Server is listening on port 53000...");

    ServerNetworkingManager manager(serverRunning);
    std::thread commandThread(commandListener, std::ref(manager));

    while (serverRunning) {
        auto* client = new sf::TcpSocket;
        if (listener.accept(*client) == sf::Socket::Done) {
            AT_INFO("New client connected");

            if (manager.addClient(client)) {
                std::thread clientThread(&ServerNetworkingManager::handleClient, &manager, client);
                clientThread.detach();
            } else {
                AT_WARN("Server full, rejecting client");
                delete client;
            }
        } else {
            delete client;
        }
    }

    commandThread.join();
    AT_INFO("Server has shut down.");

    return 0;
}