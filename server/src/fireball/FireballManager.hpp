#pragma once

#include "data/FireballData.hpp"
#include "FireballCollision.hpp"
#include <functional>

class FireballManager {
public:
    using WallDestroyedCallback = std::function<void(int, int)>;        // tileX, tileY
    using PlayerHitCallback = std::function<void(uint32_t, uint32_t)>;  // playerId, fireballOwnerId

    static constexpr uint32_t MAX_FIREBALLS = 50;
    static constexpr float MAX_LIFETIME = 5.0f; // seconds

    FireballManager() = default;

    void setCallbacks(WallDestroyedCallback wallCallback, PlayerHitCallback playerCallback) {
        onWallDestroyed = std::move(wallCallback);
        onPlayerHit = std::move(playerCallback);
    }

    uint32_t createFireball(uint32_t ownerId, const glm::vec2& position, 
                           const glm::vec2& direction, float speedMultiplier = 1.0f) {
        cleanupOldFireballs();

        if (fireballs.size() >= MAX_FIREBALLS) {
            return 0;
        }

        uint32_t id = nextFireballId++;
        fireballs.emplace_back(id, ownerId, position, direction, 
                              500.0f * speedMultiplier);
        return id;
    }

    void updateFireballs(float deltaTime, 
                        const std::vector<std::vector<int>>& map,
                        const std::vector<Player>& players) {
        for (auto& fireball : fireballs) {
            if (!fireball.active) continue;

            // Check lifetime
            auto now = std::chrono::steady_clock::now();
            float lifetime = std::chrono::duration<float>(
                now - fireball.spawnTime).count();
            
            if (lifetime > MAX_LIFETIME) {
                fireball.active = false;
                continue;
            }

            // Check collisions
            auto collision = FireballCollision::checkCollision(
                fireball, map, players, deltaTime);

            if (collision.collision) {
                handleCollision(collision, fireball);
                continue;
            }

            // Update position
            fireball.position += fireball.direction * fireball.speed * deltaTime;
        }

        cleanupOldFireballs();
    }

    [[nodiscard]] std::vector<FireballData> getActiveFireballs() const {
        std::vector<FireballData> active;
        std::copy_if(fireballs.begin(), fireballs.end(), 
                    std::back_inserter(active),
                    [](const FireballData& fb) { return fb.active; });
        return active;
    }

    void cleanupOldFireballs() {
        fireballs.erase(
            std::remove_if(fireballs.begin(), fireballs.end(),
                [](const FireballData& fb) { return !fb.active; }),
            fireballs.end()
        );
    }

private:
    void handleCollision(const CollisionResult& collision, FireballData& fireball) {
        fireball.active = false;

        if (collision.isDestructibleWall && onWallDestroyed) {
            onWallDestroyed(collision.tileX, collision.tileY);
        }

        if (collision.hitPlayerId != 0 && onPlayerHit) {
            onPlayerHit(collision.hitPlayerId, fireball.ownerId);
        }
    }

    std::vector<FireballData> fireballs;
    uint32_t nextFireballId = 1;

    WallDestroyedCallback onWallDestroyed;
    PlayerHitCallback onPlayerHit;
};