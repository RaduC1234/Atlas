#pragma once

#include <Atlas.hpp>
#include <crow.h>

#include "Packet.hpp"
#include "ServerRequest.hpp"
#include "data/DatabaseManager.hpp"

struct Lobby {
    std::unordered_map<uint64_t, Player> player;
    Registry registry;
};

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

        CROW_ROUTE(app, "/sync")([]() {
            // timestamp - no json to avoid overhead
            const Time time = Time::now();
            return crow::response(std::to_string(time.toMilliseconds()));
        });

        CROW_ROUTE(app, "/register")([this](const crow::request &req) {
            // { username, password } -> { success } or { error: username already used }
            try {
                auto requestBody = nlohmann::json::parse(req.body);

                auto username = requestBody["username"].get<std::string>();
                auto password = requestBody["password"].get<std::string>();

                std::regex usernameRegex(R"(^(?=.*\d).{4,}$)");

                if (!std::regex_match(username, usernameRegex)) {
                    return crow::response(HttpStatus::BAD_REQUEST, std::string(R"({"requestStatus": 0, "message" : "Invalid username. Must be at least 4 characters long and contain at least one number."})"));
                }

                auto players = DatabaseManager::getAll<Player>();

                bool usernameExists = std::ranges::find_if(players, [&username](const Player &player) {
                    return player.getUsername() == username;
                }) != players.end();

                if (usernameExists) {
                    return crow::response(HttpStatus::BAD_REQUEST, std::string(R"({"requestStatus": 0, "message" : "Username already exists"})"));
                }

                DatabaseManager::emplace_create<Player>(username, password, 0);
                AT_INFO("New user registered with the username {0} from remote host {1}.", username, req.remote_ip_address);
                return crow::response(HttpStatus::OK, std::string(R"({"requestStatus": 1, "message" : "Success"})"));
            } catch (const std::exception &e) {
                return crow::response(HttpStatus::BAD_REQUEST, std::string(R"({"requestStatus": 0, "message" : "Invalid request format"})"));
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
                return crow::response(HttpStatus::BAD_REQUEST, std::string(R"({"requestStatus": false, "message" : "Wrong username or password"})"));
            }

            for (const auto &[existingToken, loggedPlayer]: this->players) {
                if (loggedPlayer.getUsername() == username) {
                    return crow::response(HttpStatus::OK, std::format(R"({{"requestStatus": true, "authToken": {0}}})", existingToken));
                }
            }

            const auto authToken = Uuid::randomUUID().getMostSignificantBits();
            this->players[authToken] = *it;

            return crow::response(HttpStatus::OK, std::format(R"({{"requestStatus": 1, "authToken": "{0}"}})", authToken));
        });

        CROW_ROUTE(app, "/matchmaking")([this](const crow::request &req) {
            try {

            } catch (const std::exception &e) {
              return crow::response(400, std::string("Error: ") + e.what());
          }
        });

        CROW_ROUTE(app, "/").methods(crow::HTTPMethod::GET, crow::HTTPMethod::POST)([this](const crow::request &req) {
            try {
                auto requestBody = nlohmann::json::parse(req.body);

                const auto player = players[requestBody["authToken"].get<uint64_t>()];
                Packet packet{
                    .client = player,
                    .UUID = requestBody.at("UUID").get<uint64_t>(),
                    .timestamp = requestBody.at("timestamp").get<uint64_t>(),
                    .type = static_cast<RequestType>(requestBody.at("type").get<uint32_t>()),
                    .payload = requestBody.at("payload").get<JsonData>()
                };
            } catch (const std::exception &e) {
                return crow::response(400, std::string("Error: ") + e.what());
            }
        });


        app.loglevel(crow::LogLevel::Warning);
        app.port(port).multithreaded().run();
    }

    void stop() {
        running = false;
    }

private:
    std::vector<uint64_t> waintingPlayers;
    std::atomic_bool running;
    std::vector<Lobby> lobbies;
    std::unordered_map<uint64_t, Player> players; // token, player

    std::mutex handlerMutex;
    std::unordered_map<uint32_t, Ref<ServerRequest> > handlers;

    glm::vec3 playerPosition{0.0f};
};
