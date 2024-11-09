#include "RequestHandler.hpp"

RequestHandler::RequestHandler() {
    std::cout << "RequestHandler created" << std::endl;
}


RequestHandler::~RequestHandler() {
    std::cout << "RequestHandler destroyed" << std::endl;
}

// Define the rest of the methods
nlohmann::json RequestHandler::handleCreateLobbyRequest(const nlohmann::json& requestData) {
    std::cout << "Creating lobby with data: " << requestData.dump() << std::endl;
    nlohmann::json response = {
        {"status", "success"},
        {"message", "Lobby created successfully"}
    };
    return response;
}

nlohmann::json RequestHandler::handleJoinLobbyRequest(const nlohmann::json& requestData) {
    std::cout << "Joining lobby with data: " << requestData.dump() << std::endl;
    nlohmann::json response = {
        {"status", "success"},
        {"message", "Joined lobby successfully"}
    };
    return response;
}

nlohmann::json RequestHandler::handleLeaveLobbyRequest(const nlohmann::json& requestData) {
    std::cout << "Leaving lobby with data: " << requestData.dump() << std::endl;
    nlohmann::json response = {
        {"status", "success"},
        {"message", "Left lobby successfully"}
    };
    return response;
}

nlohmann::json RequestHandler::handleStartGameRequest(const nlohmann::json& requestData) {
    std::cout << "Starting game with data: " << requestData.dump() << std::endl;
    nlohmann::json response = {
        {"status", "success"},
        {"message", "Game started successfully"}
    };
    return response;
}