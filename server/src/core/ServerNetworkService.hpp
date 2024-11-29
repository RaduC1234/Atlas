#pragma once

#include <Atlas.hpp>
#include <crow.h>
#include <thread>
#include <mutex>
#include <queue>
#include <functional>
#include <atomic>
#include <memory>
#include <unordered_map>

class ServerNetworkService {
public:
    ServerNetworkService() : running(false) {}

    ~ServerNetworkService() {
        stop();
    }

    // Start the server
    void start(unsigned int port) {
        running = true;

        serverThread = std::thread([this, port]() {
            crow::SimpleApp app;

            CROW_ROUTE(app, "/").methods(crow::HTTPMethod::GET, crow::HTTPMethod::POST)([this](const crow::request& req) {
                try {
                    auto requestBody = nlohmann::json::parse(req.body);

                    float deltaTime = requestBody["deltaTime"].get<float>();
                    int playerId = requestBody["playerId"].get<int>();
                    bool moveForward = requestBody["moveForward"].get<bool>();
                    bool moveBackwards = requestBody["moveBackwards"].get<bool>();
                    bool moveLeft = requestBody["moveLeft"].get<bool>();
                    bool moveRight = requestBody["moveRight"].get<bool>();

                    const float movementSpeed = 150.0f;

                    if (moveForward) playerPosition.y += movementSpeed * deltaTime;
                    if (moveBackwards) playerPosition.y -= movementSpeed * deltaTime;
                    if (moveLeft) playerPosition.x -= movementSpeed * deltaTime;
                    if (moveRight) playerPosition.x += movementSpeed * deltaTime;

                    nlohmann::json responseJson = {
                        {"x", playerPosition.x},
                        {"y", playerPosition.y},
                        {"z", playerPosition.z} // Z is always 0
                    };

                    return crow::response(responseJson.dump());

                } catch (const std::exception& e) {
                    return crow::response(400, std::string("Error: ") + e.what());
                }
            });


            app.loglevel((crow::LogLevel::Warning));
            app.port(port).multithreaded().run();
        });
    }

    void stop() {
        running = false;
        if (serverThread.joinable()) {
            serverThread.join();
        }
    }

private:
    // Custom logic to process requests
    crow::response processRequest(const crow::request& req) {
        crow::response res;
        res.code = 200;  // HTTP OK
        res.body = "Processed request: " + req.body;  // Example response
        return res;
    }

    std::atomic_bool running;
    std::thread serverThread;

    std::mutex queueMutex;
    std::queue<std::pair<std::shared_ptr<crow::request>, std::function<void(crow::response)>>> requestQueue;

    std::mutex handlerMutex;
    std::unordered_map<std::string, std::function<JsonData(const JsonData&)>> handlers;
    glm::vec3 playerPosition{0.0f};
};
