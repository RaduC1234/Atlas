#pragma once

#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <nlohmann/json.hpp>
#include <iostream>
#include <entt/entt.hpp>
#include "renderer/Color.hpp"
#include "window/Keyboard.hpp"

class NetworkSystem {
public:
    NetworkSystem() : resolver_(io_context_), websocket_stream_(io_context_), connected_(false) {
        try {
            auto const results = resolver_.resolve("localhost", "8080");

            boost::asio::connect(websocket_stream_.next_layer(), results.begin(), results.end());

            websocket_stream_.handshake("localhost", "/sync_entities_ws");

            connected_ = true;
            std::cout << "WebSocket connection established." << std::endl;

            // Launch WebSocket reader thread
            reader_thread_ = std::thread(&NetworkSystem::readServerUpdates, this);
        } catch (const std::exception &e) {
            std::cerr << "WebSocket initialization error: " << e.what() << std::endl;
            connected_ = false;
        }
    }

    ~NetworkSystem() {
        if (connected_) {
            try {
                websocket_stream_.close(boost::beast::websocket::close_code::normal);
                std::cout << "WebSocket connection closed." << std::endl;
            } catch (const std::exception &e) {
                std::cerr << "Error closing WebSocket: " << e.what() << std::endl;
            }
        }

        if (reader_thread_.joinable()) {
            reader_thread_.join();
        }
    }

    // local deltaTime - only for predictions
    void update(float deltaTime, entt::registry &registry, uint64_t playerId) {
        if (connected_) {
            sendInput(playerId);
            processUpdates(registry);
        } else {
            std::cerr << "WebSocket is not connected. Update skipped." << std::endl;
        }
    }

private:
    void sendInput(uint64_t playerId) {
        nlohmann::json input;

        // Populate the input JSON
        input["playerId"] = playerId;
        input["input"] = {
            {"moveBackward", Keyboard::isKeyPressed(Keyboard::S)},
            {"moveForward", Keyboard::isKeyPressed(Keyboard::W)},
            {"moveRight", Keyboard::isKeyPressed(Keyboard::D)},
            {"moveLeft", Keyboard::isKeyPressed(Keyboard::A)},
            {"aimRotation", 0} // Replace 0 with actual rotation if available
        };

        try {
            // Write the JSON as a string to the WebSocket stream
            websocket_stream_.write(boost::asio::buffer(input.dump()));
        } catch (const std::exception &e) {
            std::cerr << "Error sending input to server: " << e.what() << std::endl;
        }
    }


    void readServerUpdates() {
        while (connected_) {
            try {
                boost::beast::flat_buffer buffer;
                websocket_stream_.read(buffer);

                auto response = nlohmann::json::parse(boost::beast::buffers_to_string(buffer.data())); {
                    std::lock_guard<std::mutex> lock(update_mutex_);
                    update_queue_.push(response);
                }
                update_condition_.notify_one();
            } catch (const std::exception &e) {
                std::cerr << "WebSocket error during read: " << e.what() << std::endl;
            }
        }
    }

    void processUpdates(entt::registry &registry) {
        std::queue<nlohmann::json> local_queue; {
            std::lock_guard<std::mutex> lock(update_mutex_);
            std::swap(local_queue, update_queue_);
        }

        while (!local_queue.empty()) {
            auto update = local_queue.front();
            local_queue.pop();

            overwriteRegistry(update, registry);
        }
    }

    void overwriteRegistry(const nlohmann::json &jsonResponse, entt::registry &registry) {
        std::unordered_map<uint64_t, entt::entity> existingEntities;

        auto view = registry.view<NetworkComponent>();
        for (auto entity: view) {
            auto &netComp = view.get<NetworkComponent>(entity);
            existingEntities[netComp.networkId] = entity;
        }

        for (const auto &entityData: jsonResponse["entities"]) {
            uint64_t networkId = entityData["networkId"].get<uint64_t>();
            entt::entity entity;

            if (existingEntities.contains(networkId)) {
                entity = existingEntities[networkId];
            } else {
                entity = registry.create();
                registry.emplace<NetworkComponent>(entity, networkId);
            }

            auto tileCode = entityData["tile-code"].get<uint32_t>();
            std::string textureName;

            if (tileCode >= EntityCode::TILE_CODE && tileCode < EntityCode::TILE_CODE + EntityCode::NEXT) {
                textureName = std::format("tile_{:04}", tileCode % TILE_CODE);
            }

            if (entityData.contains("PawnComponent")) {
                auto &pawnComp = registry.get_or_emplace<PawnComponent>(entity, entityData["PawnComponent"]["playerId"], false, false, false, false, 0.0f);
                pawnComp.moveForward = entityData["PawnComponent"]["moveForward"];
                pawnComp.moveBackwards = entityData["PawnComponent"]["moveBackwards"];
                pawnComp.moveLeft = entityData["PawnComponent"]["moveLeft"];
                pawnComp.moveRight = entityData["PawnComponent"]["moveRight"];

                if (!pawnComp.moveForward && !pawnComp.moveBackwards && !pawnComp.moveLeft && !pawnComp.moveRight) {
                    textureName = "front1";
                }
            }

            if (entityData.contains("TransformComponent")) {
                registry.emplace_or_replace<TransformComponent>(
                    entity,
                    glm::vec3(entityData["TransformComponent"]["position"][0],
                              entityData["TransformComponent"]["position"][1],
                              entityData["TransformComponent"]["position"][2]),
                    entityData["TransformComponent"]["rotation"],
                    glm::vec2(entityData["TransformComponent"]["scale"][0],
                              entityData["TransformComponent"]["scale"][1])
                );

                registry.emplace_or_replace<RenderComponent>(
                    entity,
                    textureName,
                    RenderComponent::defaultTexCoords(),
                    Color::white()
                );
            }
        }
    }

    boost::asio::io_context io_context_;
    boost::asio::ip::tcp::resolver resolver_;
    boost::beast::websocket::stream<boost::asio::ip::tcp::socket> websocket_stream_;
    bool connected_;

    std::queue<nlohmann::json> update_queue_; // Queue to store incoming updates
    std::mutex update_mutex_; // Mutex to protect access to the queue
    std::condition_variable update_condition_; // Condition variable to signal updates
    std::thread reader_thread_; // Thread to read WebSocket updates asynchronously
};
