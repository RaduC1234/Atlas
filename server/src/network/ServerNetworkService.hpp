#pragma once

#include <Atlas.hpp>
#include <crow.h>

#include "Packet.hpp"
#include "ServerRequest.hpp"
#include "data/DatabaseManager.hpp"

class Lobby {
public:
    entt::registry registry;
    std::mutex registryMutex; // Protects access to registry
    std::vector<uint64_t> players;
    uint64_t entId = 0;

    Lobby() = default;

    // ❌ Disable Copy Constructor (to prevent copying entt::registry)
    Lobby(const Lobby &) = delete;

    Lobby &operator=(const Lobby &) = delete;

    // ✅ Allow Move Constructor
    Lobby(Lobby &&other) noexcept
        : registry(std::move(other.registry)),
          players(std::move(other.players)),
          entId(other.entId) {
    }

    Lobby &operator=(Lobby &&other) noexcept {
        if (this != &other) {
            std::lock_guard<std::mutex> lock(other.registryMutex);
            registry = std::move(other.registry);
            players = std::move(other.players);
            entId = other.entId;
        }
        return *this;
    }

    bool containsPlayer(uint64_t playerId) {
        return std::ranges::find(players, playerId) != players.end();
    }

    void serializeRegistry(nlohmann::json &outJson) {
        std::lock_guard<std::mutex> lock(registryMutex); // Ensure safe access
        outJson["entities"] = nlohmann::json::array();

        auto view = registry.view<NetworkComponent, TransformComponent>();
        for (auto entity: view) {
            auto &network = view.get<NetworkComponent>(entity);
            auto &transform = registry.get<TransformComponent>(entity);

            nlohmann::json entityJson;
            entityJson["id"] = static_cast<int>(entity);
            entityJson["networkId"] = network.networkId;
            entityJson["entityType"] = static_cast<int>(network.entityType);
            entityJson["tile-code"] = static_cast<int>(network.tileCode);

            entityJson["TransformComponent"] = {
                {"position", {transform.position.x, transform.position.y, transform.position.z}},
                {"rotation", transform.rotation},
                {"scale", {transform.scale.x, transform.scale.y}}
            };

            if (registry.any_of<PawnComponent>(entity)) {
                auto &pawn = registry.get<PawnComponent>(entity);
                entityJson["PawnComponent"] = {
                    {"playerId", pawn.playerId},
                    {"moveForward", pawn.moveForward},
                    {"moveBackwards", pawn.moveBackwards},
                    {"moveLeft", pawn.moveLeft},
                    {"moveRight", pawn.moveRight},
                    {"aimRotation", pawn.aimRotation}
                };
            }

            outJson["entities"].push_back(entityJson);
        }
    }


    uint64_t nextId() {
        return entId++;
    }
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
                    return crow::response(HttpStatus::BAD_REQUEST, std::string(R"({"requestStatus": false, "message" : "Invalid username. Must be at least 4 characters long and contain at least one number."})"));
                }

                auto players = DatabaseManager::getAll<Player>();

                bool usernameExists = std::ranges::find_if(players, [&username](const Player &player) {
                    return player.getUsername() == username;
                }) != players.end();

                if (usernameExists) {
                    return crow::response(HttpStatus::BAD_REQUEST, std::string(R"({"requestStatus": false, "message" : "Username already exists"})"));
                }

                DatabaseManager::emplace_create<Player>(username, password, 0);
                AT_INFO("New user registered with the username {0} from remote host {1}.", username, req.remote_ip_address);
                return crow::response(HttpStatus::OK, std::string(R"({"requestStatus": true, "message" : "Success"})"));
            } catch (const std::exception &e) {
                return crow::response(HttpStatus::BAD_REQUEST, std::string(R"({"requestStatus": false, "message" : "Invalid request format"})"));
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

            return crow::response(HttpStatus::OK, std::format(R"({{"requestStatus": true, "authToken": "{0}"}})", authToken));
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
            if (lobbies.empty() || lobbies.back().players.size() >= 4) {
                lobbies.push_back(Lobby{}); // New lobby
            }

            Lobby &lobby = lobbies.back();
            Actor playerEntity = lobby.registry.create();
            lobby.registry.emplace<TransformComponent>(playerEntity, glm::vec3(100 * playerId, 100 * playerId, 0.0f), 0.0f, glm::vec2(100, 100));
            lobby.registry.emplace<PawnComponent>(playerEntity, static_cast<uint32_t>(playerId));
            lobby.registry.emplace<NetworkComponent>(playerEntity, lobby.nextId(), EntityType::PAWN, EntityCode::TILE_CODE + 1);

            lobby.players.push_back(playerId);

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
    std::vector<uint64_t> waintingPlayers;
    std::unordered_map<uint64_t, Player> players; // token, player

    std::vector<Lobby> lobbies;
    std::atomic_bool running;
    std::mutex handlerMutex;

    crow::response handleSyncRequest(uint64_t playerId, nlohmann::json input) {
        std::cout << input.dump() << "\n";
        for (Lobby &lobby: lobbies) {
            if (lobby.containsPlayer(playerId)) {
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
};
