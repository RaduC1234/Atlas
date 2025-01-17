#include "ClientNetworkService.hpp"

void ClientNetworkService::init(const std::string &remoteHost) {
    serverUrl = remoteHost;
    AT_INFO("Server URL is: {0}", serverUrl);
}

MapState ClientNetworkService::getMapData() {
    auto response = cpr::Get(cpr::Url{"http://localhost:8080/getMap"});

    AT_INFO("Response from server: {0}", response.text);
    if (response.status_code != 200) {
        throw std::runtime_error("Failed to fetch map data from server.");
    }

    try {
        auto mapDataJson = nlohmann::json::parse(response.text);
        return mapDataJson.get<MapState>();  // Deserialize into MapState
    } catch (const std::exception &e) {
        throw std::runtime_error("Error parsing map data: " + std::string(e.what()));
    }
}

bool ClientNetworkService::reg(const std::string &username, const std::string &password) {
    JsonData requestBody = {
        {"username", username},
        {"password", password}
    };

    auto response = cpr::Get(
        cpr::Url{"http://localhost:8080/register"},
        cpr::Header{{"Content-Type", "application/Json"}},
        cpr::Body{requestBody.dump()}
    );


    JsonData data;
    TRY_CATCH(data = JsonData::parse(response.text);,return false;);

    if (response.status_code == 200) {
        return data["requestStatus"].get<bool>();
    }

    throw std::runtime_error(data["message"].get<std::string>());
}

bool ClientNetworkService::login(const std::string &username, const std::string &password) {
    JsonData requestBody = {
        {"username", username},
        {"password", password}
    };

    auto response = cpr::Get(  // Changed from Get to Post
        cpr::Url{serverUrl + "/login"},  // Changed path to use serverUrl
        cpr::Header{{"Content-Type", "application/json"}},  // Fixed content type
        cpr::Body{requestBody.dump()}
    );

    try {
        if (response.status_code == 200) {
            JsonData data = JsonData::parse(response.text);
            loginToken = data["authToken"].get<uint64_t>();
            AT_INFO("Login successful with token: {}", loginToken);  // Added logging
            return data["requestStatus"].get<bool>();
        }
    } catch (const std::exception &e) {
        AT_ERROR("Error in login request: {}", e.what());
    }

    return false;
}

bool ClientNetworkService::joinMatchmaking(GameMode mode) {
    JsonData requestBody = {
        {"playerId", loginToken},
        {"gameMode", mode == GameMode::HEX_DUEL ? "HEX_DUEL" : "HEX_ARENA"}
    };

    auto response = cpr::Post(
        cpr::Url{serverUrl + "/matchmaking"},
        cpr::Header{{"Content-Type", "application/json"}},
        cpr::Body{requestBody.dump()}
    );

    if (response.status_code == 200) {
        AT_INFO("Successfully joined matchmaking queue");
        return true;
    }

    AT_ERROR("Failed to join matchmaking: {}", response.text);
    return false;
}

bool ClientNetworkService::leaveMatchmaking() {
    JsonData requestBody = {
        {"playerId", loginToken}
    };

    auto response = cpr::Post(
        cpr::Url{serverUrl + "/leave_queue"},
        cpr::Header{{"Content-Type", "application/json"}},
        cpr::Body{requestBody.dump()}
    );

    if (response.status_code == 200) {
        AT_INFO("Successfully left matchmaking queue");
        return true;
    }

    AT_ERROR("Failed to leave matchmaking: {}", response.text);
    return false;
}

bool ClientNetworkService::submitMatchResult(uint64_t matchId, uint64_t winnerId) {
    JsonData requestBody = {
        {"matchId", matchId},
        {"winnerId", winnerId}
    };

    auto response = cpr::Post(
        cpr::Url{serverUrl + "/match_result"},
        cpr::Header{{"Content-Type", "application/json"}},
        cpr::Body{requestBody.dump()}
    );

    if (response.status_code == 200) {
        AT_INFO("Successfully submitted match result");
        return true;
    }

    AT_ERROR("Failed to submit match result: {}", response.text);
    return false;
}

uint64_t ClientNetworkService::joinMatch() {
    auto response = cpr::Post(
            cpr::Url{serverUrl + "/join_match"},
            cpr::Header{{"Content-Type", "application/json"}}
        );

    if (response.status_code != 200) {
        AT_ERROR("Failed to join match: {}", response.text);
        throw std::runtime_error("Failed to join match: " + response.text);
    }

    try {
        uint64_t playerId = std::stoull(response.text);
        currentPlayerId = playerId;  // Store the player ID
        AT_INFO("Successfully joined match with player ID: {}", playerId);
        return playerId;
    } catch (const std::exception& e) {
        AT_ERROR("Error parsing player ID from response: {}", e.what());
        throw std::runtime_error("Error parsing player ID from response: " + std::string(e.what()));
    }
}

bool ClientNetworkService::checkMatchStatus() {
    try {
        auto response = cpr::Get(
            cpr::Url{serverUrl + "/match_status"},
            cpr::Header{{"Content-Type", "application/json"}},
            cpr::Parameters{{"playerId", std::to_string(loginToken)}}
        );

        if (response.status_code != 200) {
            if (response.status_code == 404) {
                // This is an expected case when not in queue
                return false;
            }
            AT_ERROR("Failed to check match status: Status code {}", response.status_code);
            return false;
        }

        if (response.text.empty()) {
            AT_ERROR("Empty response from server when checking match status");
            return false;
        }

        try {
            auto jsonResponse = JsonData::parse(response.text);
            bool matchFound = jsonResponse["matchFound"].get<bool>();

            if (matchFound) {
                currentMatchId = jsonResponse["matchId"].get<uint64_t>();
                AT_INFO("Match found! Match ID: {}", currentMatchId);
                return true;
            }

            return false;
        } catch (const std::exception& e) {
            AT_ERROR("Error parsing match status response: {}", e.what());
            return false;
        }
    } catch (const std::exception& e) {
        AT_ERROR("Network error checking match status: {}", e.what());
        return false;
    }
}

uint64_t ClientNetworkService::getCurrentMatchId() {
    return currentMatchId;
}

uint64_t ClientNetworkService::getCurrentPlayerId() {
    return currentPlayerId;
}
