#pragma once

#include <Atlas.hpp>
#include <crow.h>

#include "Lobby.hpp"
#include "data/DatabaseManager.hpp"

class ServerNetworkService {
public:
    ServerNetworkService() : running(false) {
    }

    ~ServerNetworkService() {
        stop();
    }

    void start(const uint32_t port) {
        running = true;
        crow::SimpleApp app;


        CROW_ROUTE(app, "/register")([this](const crow::request &req) {
            // { username, password } -> { success } or { error: username already used }
            try {
                auto requestBody = nlohmann::json::parse(req.body);

                auto username = requestBody["username"].get<std::string>();
                auto password = requestBody["password"].get<std::string>();

                std::regex usernameRegex(R"(^(?=.*\d).{4,}$)");

                if (!std::regex_match(username, usernameRegex)) {
                    return crow::response(400, std::string(R"({"requestStatus": false, "message" : "Invalid username. Must be at least 4 characters long and contain at least one number."})"));
                }

                auto players = DatabaseManager::getAll<Player>();

                bool usernameExists = std::ranges::find_if(players, [&username](const Player &player) {
                    return player.getUsername() == username;
                }) != players.end();

                if (usernameExists) {
                    return crow::response(400, std::string(R"({"requestStatus": false, "message" : "Username already exists"})"));
                }

                DatabaseManager::emplace_create<Player>(username, password, 0);
                AT_INFO("New user registered with the username {0} from remote host {1}.", username, req.remote_ip_address);
                return crow::response(200, std::string(R"({"requestStatus": true, "message" : "Success"})"));
            } catch (const std::exception &e) {
                return crow::response(400, std::string(R"({"requestStatus": false, "message" : "Invalid request format"})"));
            }
        });


        CROW_ROUTE(app, "/login")([this](const crow::request &req) {
            auto requestBody = nlohmann::json::parse(req.body);

            auto username = requestBody["username"].get<std::string>();
            auto password = requestBody["password"].get<std::string>();

            auto players = DatabaseManager::getAll<Player>();

            const auto it = std::ranges::find_if(players, [&](const Player &player) {
                return player.getUsername() == username;
            });

            if (it == players.end() || it->getPassword() != password) {
                return crow::response(400, std::string(R"({"requestStatus": false, "message" : "Wrong username or password"})"));
            }

            for (const auto &[existingToken, loggedPlayer]: this->players) {
                if (loggedPlayer.getUsername() == username) {
                    return crow::response(200, std::format(R"({{"requestStatus": true, "authToken": {0}}})", existingToken));
                }
            }

            const auto authToken = Uuid::randomUUID().getMostSignificantBits();
            this->players[authToken] = *it;

            return crow::response(200, std::format(R"({{"requestStatus": true, "authToken": "{0}"}})", authToken));
        });

        CROW_ROUTE(app, "/matchmaking")([this](const crow::request &req) {
            try {
            } catch (const std::exception &e) {
                return crow::response(400, std::string("Error: ") + e.what());
            }
        });

        // Matchmaking (Create Lobbies)
        CROW_ROUTE(app, "/join_match")([this](const crow::request &req) {
            std::lock_guard<std::mutex> lock(handlerMutex);

            uint64_t playerId = generateUniqueId(); // Simple unique ID generator
            if (lobbies.empty() || lobbies.back().getPlayerList().size() >= 4) {
                lobbies.push_back(Lobby{}); // New lobby
            }

            Lobby &lobby = lobbies.back();
            Actor playerEntity = lobby.getRegistry().create();
            lobby.getRegistry().emplace<TransformComponent>(playerEntity, glm::vec3(100 * playerId, 100 * playerId, 0.0f), 0.0f, glm::vec2(100, 100));
            lobby.getRegistry().emplace<PawnComponent>(playerEntity, static_cast<uint32_t>(playerId));
            lobby.getRegistry().emplace<NetworkComponent>(playerEntity, lobby.nextId(), EntityType::PAWN, static_cast<uint32_t>(0));

            lobby.getPlayerList().push_back(playerId);

            return crow::response(std::to_string(playerId)); // Client gets player ID
        });

        CROW_ROUTE(app, "/sync_entities").methods(crow::HTTPMethod::POST)([&](const crow::request &req) {
            try {
                auto requestBody = nlohmann::json::parse(req.body);
                uint64_t playerId = requestBody["playerId"].get<uint64_t>();

                return handleSyncRequest(playerId, requestBody["input"]);
            } catch (const std::exception &e) {
                return crow::response(400, std::string("Error parsing request: ") + e.what());
            }
        });


        app.loglevel(crow::LogLevel::Warning);
        app.port(port).multithreaded().run();
    }

    void stop() {
        running = false;
    }

private:

    crow::response handleSyncRequest(uint64_t playerId, nlohmann::json input) {
        for (Lobby &lobby: lobbies) {
            if (lobby.containsPlayer(playerId)) {

                auto view = lobby.getRegistry().view<PawnComponent, TransformComponent>();
                for (auto entity : view) {
                    auto &pawn = view.get<PawnComponent>(entity);
                    auto &transform = view.get<TransformComponent>(entity);

                    if (pawn.playerId == playerId) {

                        pawn.moveForward = input.value("moveForward", false);
                        pawn.moveBackwards = input.value("moveBackwards", false);
                        pawn.moveLeft = input.value("moveLeft", false);
                        pawn.moveRight = input.value("moveRight", false);
                        pawn.aimRotation = input.value("aimRotation", 0.0f);

                        float speed = 25.0f;
                        if (pawn.moveForward) transform.position.y += speed;
                        if (pawn.moveBackwards) transform.position.y -= speed;
                        if (pawn.moveLeft) transform.position.x -= speed;
                        if (pawn.moveRight) transform.position.x += speed;
                    }
                }

                nlohmann::json responseJson;
                lobby.serializeRegistry(responseJson);
                return crow::response(responseJson.dump());
            }
        }
        return crow::response(404, "Invalid player ID");
    }

    uint64_t generateUniqueId() {
        static uint64_t idCounter = 1;
        return idCounter++;
    }

    std::vector<uint64_t> waintingPlayers;
    std::unordered_map<uint64_t, Player> players; // token, player

    std::vector<Lobby> lobbies;
    std::atomic_bool running;
    std::mutex handlerMutex;
};
