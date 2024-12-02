#pragma once

#include <Atlas.hpp>
//#include <cpr/cpr.h>

class NetworkSystem {
public:
    void update(float deltaTime, entt::registry &registry) {
        auto view = registry.view<TransformComponent, PawnComponent>();

        for (auto entity : view) {
            auto &transform = view.get<TransformComponent>(entity);
            auto &pawn = view.get<PawnComponent>(entity);

            std::thread([deltaTime, pawn, &transform, &registry, entity]() {
                try {
                    // Serialize input data to JSON
                    nlohmann::json inputJson = {
                        {"deltaTime", deltaTime},
                        {"playerId", pawn.playerId},
                        {"moveForward", pawn.moveForward},
                        {"moveBackwards", pawn.moveBackwards},
                        {"moveLeft", pawn.moveLeft},
                        {"moveRight", pawn.moveRight}
                    };

                    /*cpr::Response response = cpr::Post(
                        cpr::Url{"http://localhost:8080/"},
                        cpr::Header{{"Content-Type", "application/json"}},
                        cpr::Body{inputJson.dump()}
                    );

                    if (response.error) {
                        std::cerr << "Failed to send input to server: " << response.error.message << "\n";
                        return;
                    }

                    if (response.status_code == 200) {
                        auto responseJson = nlohmann::json::parse(response.text);
                        glm::vec3 newPosition = {
                            responseJson["x"].get<float>(),
                            responseJson["y"].get<float>(),
                            responseJson["z"].get<float>()
                        };

                        registry.get<TransformComponent>(entity).position = newPosition;
                    } else {
                        std::cerr << "Server error: " << response.status_code << "\n";
                        std::cerr << "Response: " << response.text << "\n";
                    }*/
                } catch (const std::exception &e) {
                    std::cerr << "Exception in network thread: " << e.what() << "\n";
                }
            }).detach();
        }
    }
};
