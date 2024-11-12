#include "CommandHandler.hpp"


CommandHandler::CommandHandler() {
    // Constructor implementation (if any)
    std::cout << "CommandHandler created." << std::endl;
}

CommandHandler::~CommandHandler() {
    // Destructor implementation (if any)
    std::cout << "CommandHandler destroyed." << std::endl;
}

void CommandHandler::handleCommand(const nlohmann::json& commandJson) {
    std::string command = commandJson["command"];
    auto params = commandJson["params"];

    if (command == "start_game") {
        handleStartGame(params);
    } else if (command == "pause_game") {
        handlePauseGame(params);
    } else if (command == "stop_game") {
        handleStopGame(params);
    } else {
        handleCustomCommand(command, params);
    }
}

void CommandHandler::handleStartGame(const nlohmann::json& params) {
    std::cout << "Starting game with parameters: " << params.dump() << std::endl;
}

void CommandHandler::handlePauseGame(const nlohmann::json& params) {
    std::cout << "Pausing game" << std::endl;
}

void CommandHandler::handleStopGame(const nlohmann::json& params) {
    std::cout << "Stopping game" << std::endl;
}

void CommandHandler::handleCustomCommand(const std::string& command, const nlohmann::json& params) {
    std::cout << "Handling custom command: " << command << std::endl;
}
