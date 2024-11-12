// CommandHandler.hpp
#pragma once
#include "CoreServer.hpp"
class CommandHandler {
public:
    CommandHandler();
    ~CommandHandler();

    void handleCommand(const nlohmann::json& commandJson);

private:
    void handleStartGame(const nlohmann::json& params);
    void handlePauseGame(const nlohmann::json& params);
    void handleStopGame(const nlohmann::json& params);
    void handleCustomCommand(const std::string& command, const nlohmann::json& params);
};

