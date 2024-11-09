// RequestHandler.hpp
#pragma once
#include "CoreServer.hpp"
class RequestHandler {
public:
    RequestHandler();
    ~RequestHandler();

    nlohmann::json handleCreateLobbyRequest(const nlohmann::json& requestData);
    nlohmann::json handleJoinLobbyRequest(const nlohmann::json& requestData);
    nlohmann::json handleLeaveLobbyRequest(const nlohmann::json& requestData);
    nlohmann::json handleStartGameRequest(const nlohmann::json& requestData);

private:

};
