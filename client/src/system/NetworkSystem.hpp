#pragma once

#include <Atlas.hpp>
#include <cpr/cpr.h>

class NetworkSystem {
public:
    NetworkSystem() {

        // sync thread
        std::thread([this]() {
             constexpr int syncInterval = 5000;
             int retryCounter = 1;

             while (isRunning.load()) {
                 try {
                     Time t1 = Time::now();
                     cpr::Response response = cpr::Get(cpr::Url{"http://localhost:8080/sync"});
                     Time t2 = Time::now();

                     if (response.status_code == 200) {
                         int64_t serverTimeMs = std::stoll(response.text);
                         auto serverTime = Time(std::chrono::milliseconds(serverTimeMs));
                         auto roundTripTime = t2 - t1;

                         auto clientToServerDelay = roundTripTime.count() / 2;

                         Time adjustedTime = serverTime;
                         adjustedTime.addMilliseconds(clientToServerDelay);

                         AT_TRACE("Synchronized Time: {0} ms. Ping: {1} ms. Client to server delay {2} ms.", adjustedTime.toMilliseconds(), roundTripTime.count(), clientToServerDelay);
                     } else {
                         if (retryCounter < 5) {
                             AT_ERROR("Failed to sync with server for {0} attempt. HTTP Status: {1}. Retrying in {2} milliseconds...", retryCounter++, response.status_code, syncInterval);
                         }
                         AT_FATAL("Failed to sync with the server after {0} attempts. Exiting...", retryCounter);
                     }
                 } catch (const std::exception &e) {
                     AT_FATAL("Exception during synchronization: {0}", e.what());
                 }
                 std::this_thread::sleep_for(std::chrono::milliseconds(syncInterval));
             }
         }).detach();
    }

    // game thread
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

                    cpr::Response response = cpr::Post(
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
                    }
                } catch (const std::exception &e) {
                    std::cerr << "Exception in network thread: " << e.what() << "\n";
                }
            }).detach();
        }
    }
private:
    std::atomic_int64_t syncedTimestamp{0};
    std::atomic_bool isRunning{true};
};
