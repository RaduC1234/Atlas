#pragma once

#include <../../../engine/src/Atlas.hpp>
#include <chrono>

struct FireballData {
    uint32_t id;               // Unique identifier for this fireball
    uint32_t ownerId;          // ID of player who shot the fireball
    glm::vec2 position;        // Current position
    glm::vec2 direction;       // Normalized direction vector
    float speed;               // Current speed
    bool active;               // Whether fireball is still active
    std::chrono::steady_clock::time_point spawnTime; // When fireball was created

    static constexpr int FIREBALL_DAMAGE = 1;  // Fixed damage of 1 HP player has  3 HP / 3 lives

    FireballData(uint32_t id, uint32_t ownerId, const glm::vec2& pos,
                const glm::vec2& dir, float speed = 500.0f)
        : id(id)
        , ownerId(ownerId)
        , position(pos)
        , direction(glm::normalize(dir))
        , speed(speed)
        , active(true)
        , spawnTime(std::chrono::steady_clock::now()) {}

    // Serialization support for network transmission , or so i hope
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(FireballData, id, ownerId, position, direction, speed, active)
};