#pragma once

#include <Atlas.hpp>
#include <crow.h>

#include "Lobby.hpp"
#include "data/DatabaseManager.hpp"
#include "matchmaking/MatchmakingManager.hpp"

enum class GameMode {
    HEX_DUEL,    // 1v1
    HEX_ARENA    // 1v1v1v1
};


struct QueuedPlayer {
    uint64_t playerId;
    Player player;
    GameMode mode;
    std::chrono::system_clock::time_point queueTime;
};

class ServerNetworkService {
public:
    ServerNetworkService() : running(false) {
        MatchmakingManager::init();
    }

    ~ServerNetworkService() {
        stop();
        if (matchmakingThread.joinable()) {
            matchmakingThread.join();
        }
        MatchmakingManager::shutdown();
    }

    void start(const uint32_t port) {
        running = true;
        crow::SimpleApp app;

        matchmakingThread = std::thread(&ServerNetworkService::matchmakingLoop, this);

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

           const auto authToken = Uuid::randomUUID().getMostSignificantBits() & 0x7FFFFFFFFFFFFFF;
           this->players[authToken] = *it;

           return crow::response(200, std::format(R"({{"requestStatus": true, "authToken": {0}}})", authToken));
       });

        CROW_ROUTE(app, "/matchmaking")
             .methods(crow::HTTPMethod::POST)([this](const crow::request& req) {
             try {
                 auto body = crow::json::load(req.body);
                 uint64_t authToken = body["playerId"].i();  // This is actually the auth token
                 std::string gameModeStr = body["gameMode"].s();
                 GameMode mode = gameModeStr == "HEX_DUEL" ? GameMode::HEX_DUEL : GameMode::HEX_ARENA;

                 AT_INFO("Received matchmaking request with auth token: {}", authToken);

                 // Find player using auth token
                 auto playerIt = players.find(authToken);
                 if (playerIt == players.end()) {
                     AT_ERROR("Player with auth token {} not found in players map", authToken);
                     return crow::response(400, "Player not found");
                 }

                 {
                     std::lock_guard<std::mutex> lock(queueMutex);

                     // Check if player is already in queue using their auth token
                     auto it = std::find_if(matchmakingQueue.begin(), matchmakingQueue.end(),
                         [authToken](const QueuedPlayer& qp) { return qp.playerId == authToken; });

                     if (it != matchmakingQueue.end()) {
                         return crow::response(400, "Already in queue");
                     }

                     // Add to queue with full player object
                     matchmakingQueue.push_back({
                         authToken,  // Use auth token as player ID for now
                         playerIt->second,  // Store the full Player object
                         mode,
                         std::chrono::system_clock::now()
                     });

                     AT_INFO("Player with auth token {} added to {} queue", authToken,
                            mode == GameMode::HEX_DUEL ? "HEX_DUEL" : "HEX_ARENA");
                 }

                 return crow::response(200, "Added to matchmaking queue");

             } catch (const std::exception& e) {
                 return crow::response(400, std::string("Error: ") + e.what());
             }
         });

        CROW_ROUTE(app, "/leave_queue")
            .methods(crow::HTTPMethod::POST)([this](const crow::request& req) {
            try {
                auto body = crow::json::load(req.body);
                uint64_t authToken = body["playerId"].i();

                // Verify the auth token is valid
                if (players.find(authToken) == players.end()) {
                    return crow::response(400, "Invalid auth token");
                }

                {
                    std::lock_guard<std::mutex> lock(queueMutex);
                    removeFromQueue({authToken});
                }

                return crow::response(200, "Removed from queue");

            } catch (const std::exception& e) {
                return crow::response(400, std::string("Error: ") + e.what());
            }
        });

        // Matchmaking (Create Lobbies) - test only
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
            lobby.getRegistry().emplace<NetworkComponent>(playerEntity, lobby.nextId());

            lobby.getPlayerList().push_back(playerId);

            return crow::response(std::to_string(playerId)); // Client gets player ID
        });

        CROW_ROUTE(app, "/match_status")
            .methods(crow::HTTPMethod::GET)([this](const crow::request& req) {
            try {
                // Get playerId from query parameter
                auto playerIdStr = req.url_params.get("playerId");
                if (!playerIdStr) {
                    return crow::response(400, R"({"error": "Missing playerId parameter"})");
                }

                uint64_t playerId = std::stoull(playerIdStr);

                // Check if player is in any lobby
                for (const auto& lobby : lobbies) {
                    auto& playerList = lobby.getPlayerList();
                    if (std::find(playerList.begin(), playerList.end(), playerId) != playerList.end()) {
                        nlohmann::json response = {
                            {"matchFound", true},
                            {"matchId", lobby.getId()}
                        };
                        return crow::response(200, response.dump());
                    }
                }

                // Check if player is still in queue
                {
                    std::lock_guard<std::mutex> lock(queueMutex);
                    auto it = std::find_if(matchmakingQueue.begin(), matchmakingQueue.end(),
                        [playerId](const QueuedPlayer& qp) { return qp.playerId == playerId; });

                    if (it == matchmakingQueue.end()) {
                        // Player is neither in queue nor in a match
                        return crow::response(404, R"({"error": "Player not found in queue or match"})");
                    }
                }

                // Player is still in queue
                return crow::response(200, R"({"matchFound": false})");

            } catch (const std::exception& e) {
                return crow::response(400,
                    nlohmann::json{{"error", std::string("Error: ") + e.what()}}.dump());
            }
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

        CROW_ROUTE(app, "/match_result")
           .methods(crow::HTTPMethod::POST)([this](const crow::request &req) {
           try {
               auto body = crow::json::load(req.body);
               uint64_t matchId = body["matchId"].i();
               uint64_t winnerId = body["winnerId"].i();

               // Find the match lobby
               auto lobbyIt = std::find_if(lobbies.begin(), lobbies.end(),
                   [matchId](const Lobby& lobby) {
                       return lobby.getId() == matchId;
                   });

               if (lobbyIt == lobbies.end()) {
                   return crow::response(404, "Match not found");
               }

               // Get all players in the match
               std::vector<Player> matchPlayers;
               for (uint64_t playerId : lobbyIt->getPlayerList()) {
                   auto playerIt = players.find(playerId);
                   if (playerIt != players.end()) {
                       matchPlayers.push_back(playerIt->second);
                   }
               }

               // Find winner
               auto winnerIt = std::find_if(matchPlayers.begin(), matchPlayers.end(),
                   [winnerId](const Player& p) { return p.getId() == winnerId; });

               if (winnerIt == matchPlayers.end()) {
                   return crow::response(400, "Invalid winner ID");
               }

               // Update ratings
               MatchmakingManager::updateRatings(matchPlayers, *winnerIt);

               // Clean up the lobby
               lobbies.erase(lobbyIt);

               return crow::response(200, "Match results processed");

           } catch (const std::exception& e) {
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
    std::atomic_bool running;
    std::mutex handlerMutex;
    std::mutex queueMutex;
    std::thread matchmakingThread;
    std::vector<Lobby> lobbies;
    std::vector<QueuedPlayer> matchmakingQueue;
    std::unordered_map<uint64_t, Player> players;

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

                        constexpr float speed = 50.0f;
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

    void matchmakingLoop() {
        while (running) {
            std::lock_guard<std::mutex> lock(queueMutex);
            tryMatchDuel();
            tryMatchArena();
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }

    void tryMatchDuel() {
        auto duelPlayers = getPlayersInQueue(GameMode::HEX_DUEL);

        // Early return if we don't have enough players
        if (duelPlayers.size() < 2) return;

        for (size_t i = 0; i < duelPlayers.size(); i++) {
            for (size_t j = i + 1; j < duelPlayers.size(); j++) {
                // Create a vector with exactly 2 players for the duel
                std::vector<Player> playerPair = {duelPlayers[i].player, duelPlayers[j].player};
                auto quality = MatchmakingManager::evaluateMatch(playerPair);

                if (quality.isValid && quality.quality >= 0.7) {
                    // Create a vector with exactly 2 QueuedPlayer objects
                    std::vector<QueuedPlayer> matchedPair = {duelPlayers[i], duelPlayers[j]};
                    createMatch(matchedPair, GameMode::HEX_DUEL);

                    // Remove only the matched players from queue
                    std::vector<uint64_t> matchedIds = {duelPlayers[i].playerId, duelPlayers[j].playerId};
                    removeFromQueue(matchedIds);
                    return; // Exit after creating a match
                }
            }
        }
    }

    void tryMatchArena() {
        auto arenaPlayers = getPlayersInQueue(GameMode::HEX_ARENA);

        // Early return if we don't have enough players
        if (arenaPlayers.size() < 4) return;

        // Look for groups of exactly 4 players
        for (size_t i = 0; i <= arenaPlayers.size() - 4; i++) {
            // Take exactly 4 players for evaluation
            std::vector<QueuedPlayer> potentialMatch(arenaPlayers.begin() + i, arenaPlayers.begin() + i + 4);

            std::vector<Player> playerGroup;
            for (const auto& qp : potentialMatch) {
                playerGroup.push_back(qp.player);
            }

            auto quality = MatchmakingManager::evaluateMatch(playerGroup);

            if (quality.isValid && quality.quality >= 0.6) {
                createMatch(potentialMatch, GameMode::HEX_ARENA);

                // Remove exactly these 4 players from queue
                std::vector<uint64_t> playerIds;
                for (const auto& player : potentialMatch) {
                    playerIds.push_back(player.playerId);
                }
                removeFromQueue(playerIds);
                return; // Exit after creating a match
            }
        }
    }

    // Helper function to validate match requirements
    static bool validateMatchRequirements(const std::vector<QueuedPlayer>& players, GameMode mode) {
        size_t requiredPlayers = (mode == GameMode::HEX_DUEL) ? 2 : 4;
        if (players.size() != requiredPlayers) {
            AT_ERROR("Invalid number of players for game mode {}. Required: {}, Got: {}",
                    mode == GameMode::HEX_DUEL ? "HEX_DUEL" : "HEX_ARENA",
                    requiredPlayers, players.size());
            return false;
        }
        return true;
    }

    std::vector<QueuedPlayer> getPlayersInQueue(GameMode mode) {
        std::vector<QueuedPlayer> result;
        std::copy_if(matchmakingQueue.begin(), matchmakingQueue.end(),
                    std::back_inserter(result),
                    [mode](const QueuedPlayer& player) { return player.mode == mode; });
        return result;
    }

    void removeFromQueue(const std::vector<uint64_t>& playerIds) {
        matchmakingQueue.erase(
            std::remove_if(matchmakingQueue.begin(), matchmakingQueue.end(),
                        [&](const QueuedPlayer& player) {
                            return std::find(playerIds.begin(), playerIds.end(),
                                          player.playerId) != playerIds.end();
                        }),
            matchmakingQueue.end());
    }

     void createMatch(const std::vector<QueuedPlayer>& matchedPlayers, GameMode mode) {
        std::lock_guard<std::mutex> lock(handlerMutex);

        // Verify correct number of players for game mode
        size_t expectedPlayers = (mode == GameMode::HEX_DUEL) ? 2 : 4;
        if (matchedPlayers.size() != expectedPlayers) {
            AT_ERROR("Invalid number of players for game mode. Expected: {}, Got: {}",
                    expectedPlayers, matchedPlayers.size());
            return;
        }

        // Create lobby and add authorized players to its list
        lobbies.emplace_back();
        Lobby& lobby = lobbies.back();

        // Add all matched players to authorized list
        for (const auto& player : matchedPlayers) {
            lobby.addPlayer(player.playerId);
        }

        // Create match record
        Match matchRecord;
        std::vector<int> playerIds;
        for (const auto& player : matchedPlayers) {
            Player updatedPlayer = player.player;
            updatedPlayer.setMatchId(lobby.getId());
            DatabaseManager::update(updatedPlayer);
            playerIds.push_back(static_cast<int>(player.playerId));
        }

        matchRecord.setPlayerIds(playerIds);
        DatabaseManager::create(matchRecord);

        AT_INFO("Created new {} lobby for {} players",
                mode == GameMode::HEX_DUEL ? "HEX_DUEL" : "HEX_ARENA",
                matchedPlayers.size());
    }

    crow::response handleJoinMatch(const crow::request& req) {
        std::lock_guard<std::mutex> lock(handlerMutex);

        try {
            auto body = crow::json::load(req.body);
            uint64_t playerId = body["playerId"].i();

            // Find the lobby this player belongs to
            for (auto& lobby : lobbies) {
                if (lobby.containsPlayer(playerId)) {
                    auto& playerList = lobby.getPlayerList();
                    size_t playerIndex = std::find(playerList.begin(), playerList.end(), playerId) - playerList.begin();

                    // Create player entity with position based on index
                    Actor playerEntity = lobby.getRegistry().create();
                    glm::vec3 position;

                    // Position based on total expected players
                    if (playerList.size() <= 2) { // HEX_DUEL
                        position = (playerIndex == 0) ? glm::vec3(-300, 0, 0) : glm::vec3(300, 0, 0);
                    } else { // HEX_ARENA
                        switch (playerIndex) {
                            case 0: position = glm::vec3(-300, -300, 0); break;
                            case 1: position = glm::vec3(300, -300, 0); break;
                            case 2: position = glm::vec3(-300, 300, 0); break;
                            case 3: position = glm::vec3(300, 300, 0); break;
                            default: position = glm::vec3(0, 0, 0); break;
                        }
                    }

                    lobby.getRegistry().emplace<TransformComponent>(playerEntity, position, 0.0f, glm::vec2(100, 100));
                    lobby.getRegistry().emplace<PawnComponent>(playerEntity, playerId);
                    lobby.getRegistry().emplace<NetworkComponent>(playerEntity, lobby.nextId());

                    return crow::response(200, std::to_string(playerId));
                }
            }

            return crow::response(404, "No matching lobby found for player");
        } catch (const std::exception& e) {
            return crow::response(400, std::string("Error: ") + e.what());
        }
    }

};