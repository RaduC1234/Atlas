#pragma once

#include "FireballData.hpp"
#include "../data/Player.hpp"

struct CollisionResult {
    bool collision{false};         // Whether collision occurred
    glm::vec2 point{0.0f};        // Point of collision
    int tileX{-1}, tileY{-1};     // Map tile coordinates (if wall collision)
    uint32_t hitPlayerId{0};       // ID of hit player (if player collision)
    bool isDestructibleWall{false};// Whether hit wall is destructible
};

class FireballCollision {
public:
    static const float PLAYER_RADIUS;
    static const float FIREBALL_RADIUS;
    static const float TILE_SIZE;

    static CollisionResult checkCollision(
        const FireballData& fireball,
        const std::vector<std::vector<int>>& map,
        const std::vector<Player>& players,
        float deltaTime
    ) {
        CollisionResult result;
        
        // Calculate next position
        glm::vec2 nextPos = fireball.position + 
            fireball.direction * fireball.speed * deltaTime;

        // Check map boundaries first
        if (checkMapBoundaries(nextPos, map, result)) {
            return result;
        }

        // Check wall collisions
        if (checkWallCollision(nextPos, map, result)) {
            return result;
        }

        // Check player collisions
        if (checkPlayerCollisions(nextPos, fireball, players, result)) {
            return result;
        }

        return result;
    }

private:
    static bool checkMapBoundaries(
        const glm::vec2& pos,
        const std::vector<std::vector<int>>& map,
        CollisionResult& result
    ) {
        int currentTileX = static_cast<int>(pos.x / TILE_SIZE);
        int currentTileY = static_cast<int>(pos.y / TILE_SIZE);

        if (currentTileX < 0 || currentTileY < 0 || 
            currentTileY >= map.size() || 
            currentTileX >= map[0].size()) {
            result.collision = true;
            result.point = pos;
            return true;
        }
        return false;
    }

    static bool checkWallCollision(
        const glm::vec2& pos,
        const std::vector<std::vector<int>>& map,
        CollisionResult& result
    ) {
        int currentTileX = static_cast<int>(pos.x / TILE_SIZE);
        int currentTileY = static_cast<int>(pos.y / TILE_SIZE);

        int tileType = map[currentTileY][currentTileX];
        if (tileType == 3 || tileType == 4) { // Wall types
            result.collision = true;
            result.tileX = currentTileX;
            result.tileY = currentTileY;
            result.isDestructibleWall = (tileType == 3);
            result.point = pos;
            return true;
        }
        return false;
    }

    static bool checkPlayerCollisions(
        const glm::vec2& pos,
        const FireballData& fireball,
        const std::vector<Player>& players,
        CollisionResult& result
    ) {
        for (const auto& player : players) {
            if (player.getId() == fireball.ownerId) continue;

            // Get player position from registry
            glm::vec2 playerPos(0.0f); // player data steal
            
            // Circle collision check
            float dx = playerPos.x - pos.x;
            float dy = playerPos.y - pos.y;
            float distSq = dx * dx + dy * dy;
            float minDist = PLAYER_RADIUS + FIREBALL_RADIUS;

            if (distSq < minDist * minDist) {
                result.collision = true;
                result.hitPlayerId = player.getId();
                result.point = pos;
                return true;
            }
        }
        return false;
    }
};

// Define static constants will change if its wrong later when im done in client to render the fireball with that animation
const float FireballCollision::PLAYER_RADIUS = 30.0f;
const float FireballCollision::FIREBALL_RADIUS = 15.0f;
const float FireballCollision::TILE_SIZE = 100.0f;