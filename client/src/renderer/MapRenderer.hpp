#pragma once
#include "map/MapState.hpp"
#include "entity/Entity.hpp"
#include <unordered_map>

class MapRenderer {
public:
    static void renderMap(const MapState& map, Registry& registry) {
        // Render each tile with a low z-index
        for (uint32_t y = 0; y < MapState::MAP_HEIGHT; y++) {
            for (uint32_t x = 0; x < MapState::MAP_WIDTH; x++) {
                renderTile(map.at(x, y), x, y, registry);
            }
        }

        // Debug render for spawn points
        if (RENDER_SPAWN_POINTS) {
            for (const auto& spawn : map.spawnPoints) {
                renderSpawnPoint(spawn, registry);
            }
        }
    }

    static void refreshTile(const MapState& map, uint32_t x, uint32_t y, Registry& registry) {
        // Find and delete old tile
        auto view = registry.view<TransformComponent>();
        for (auto entity : view) {
            auto& transform = view.get<TransformComponent>(entity);
            if (transform.position.x == x * MapState::TILE_SIZE &&
                transform.position.y == y * MapState::TILE_SIZE &&
                transform.position.z <= 2.0f) {  // Only delete tile entities
                registry.destroy(entity);
                break;
            }
        }

        // Render new tile
        renderTile(map.at(x, y), x, y, registry);
    }

private:
    static constexpr bool RENDER_SPAWN_POINTS = true;
    static constexpr float TILE_LAYER_Z = 1.5f;
    static constexpr float SPAWN_POINT_LAYER_Z = 2.0f;

    // Static utility method to pad tile numbers with zeros
    static std::string getPaddedTileNumber(int tileNum, int padLength = 4) {
        std::ostringstream oss;
        oss << std::setw(padLength) << std::setfill('0') << tileNum;
        return oss.str();
    }

    // Helper function to convert glm::vec4 to a string for logging
    static std::string glmVec4ToString(const glm::vec4& vec) {
        return "(" + std::to_string(vec.r) + ", " +
               std::to_string(vec.g) + ", " +
               std::to_string(vec.b) + ", " +
               std::to_string(vec.a) + ")";
    }

    // Function to get the color based on the tile type
    static glm::vec4 getTileColorByType(TileType type) {
        switch (type) {
            case TileType::Path:               return glm::vec4(0.6f, 0.3f, 0.1f, 1.0f);  // Brown for path
            case TileType::Grass:              return glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);  // Green for grass
            case TileType::Bush:               return glm::vec4(0.1f, 0.4f, 0.1f, 1.0f);  // Dark green for bush
            case TileType::DestructibleWall:   return glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);  // Gray for destructible walls
            case TileType::IndestructibleWall: return glm::vec4(0.3f, 0.3f, 0.3f, 1.0f);  // Dark gray for indestructible walls
            case TileType::HealthPickup:       return glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);  // Blue for health pickup
            case TileType::SpeedBoost:         return glm::vec4(1.0f, 0.0f, 1.0f, 1.0f);  // Pink for speed boost
            case TileType::HiddenBomb:         return glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);  // Red for hidden bomb
            default:                          return glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);  // Default white for unknown tile types
        }
    }

    static void renderTile(const MapTile& tile, uint32_t x, uint32_t y, Registry& registry) {
        glm::vec4 color = getTileColorByType(tile.type);  // Get the color based on the tile type
        

        Actors::createStaticProp(
            registry,
            {glm::vec3(x * MapState::TILE_SIZE, y * MapState::TILE_SIZE, TILE_LAYER_Z), 0.0f, glm::vec2(MapState::TILE_SIZE, MapState::TILE_SIZE)},
            {"", RenderComponent::defaultTexCoords(), color}  // Render a colored square for each tile
        );
    }

    static void renderSpawnPoint(const SpawnPoint& spawn, Registry& registry) {
        // Set spawn points to z-index between tiles and player
        glm::vec3 position(spawn.position.x * MapState::TILE_SIZE,
                          spawn.position.y * MapState::TILE_SIZE,
                          SPAWN_POINT_LAYER_Z);
        glm::vec2 size(MapState::TILE_SIZE * 0.5f);

        Actors::createStaticProp(
            registry,
            {position, spawn.rotation, size},
            {"spawn_marker", RenderComponent::defaultTexCoords(),
             spawn.occupied ? glm::vec4(1.0f, 0.0f, 0.0f, 0.5f) :
                            glm::vec4(0.0f, 1.0f, 0.0f, 0.5f)}
        );
    }

    static std::string getTileTextureKey(TileType type) {
        // Mapping of TileType to tile numbers (for textures)
        static const std::unordered_map<TileType, int> tileMappings = {
            {TileType::Path,             0},
            {TileType::Grass,             49},
            {TileType::Bush,              2},
            {TileType::DestructibleWall,   37},
            {TileType::IndestructibleWall, 37},
            {TileType::HealthPickup,      51},
            {TileType::SpeedBoost,        52},
            {TileType::HiddenBomb,        53}
        };

        // Find the tile number for the given type
        auto it = tileMappings.find(type);
        if (it != tileMappings.end()) {
            return "tile_" + getPaddedTileNumber(it->second);
        }

        // Fallback to default
        return "tile_" + getPaddedTileNumber(0);
    }

    static glm::vec4 getTileColor(const MapTile& tile) {
        glm::vec4 color(1.0f);

        // Apply visual effects based on tile state
        if (tile.damaged) {
            color = glm::vec4(0.7f, 0.7f, 0.7f, 1.0f);
        }

        if (!tile.revealed) {
            color.a = 0.5f;
        }

        if (HIGHLIGHT_WALKABLE && tile.walkable) {
            color.g *= 1.2f;  // Make walkable tiles slightly greener
        }

        return color;
    }

    static constexpr bool HIGHLIGHT_WALKABLE = false;
};
