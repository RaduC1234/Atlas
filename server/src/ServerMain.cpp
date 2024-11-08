#include "Atlas.hpp"
#include "core/ServerNetworkManager.hpp"
#include <SFML/Network.hpp>



std::atomic<bool> serverRunning{true}; // TODO: Fa un thread petnru fiecare lobby.

void commandListener(ServerNetworkingManager& manager) { //TODO: Command module with command design patten
    std::string command;
    while (serverRunning) {
        std::getline(std::cin, command);
        manager.handleCommand(command);
    }
}

int main(int argc, char** argv) { // This is bad code. Refractor Andre
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


//    CommandHandler handler; // pe clasa
//    handle.run(); // pe thread separat
//
//    while(true) {
//        for (lobby: lobies) {
//            lobbymanager; // https://github.com/RaduC1234/nio-Server/blob/master/src/main/java/team/JavaTeens/ServerCommand/CommandHandler.java
//        }
//    }


    return 0;
}
