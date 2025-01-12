#pragma once

#include <Atlas.hpp>
#include "map/MapState.hpp"
#include "map/MapTile.hpp"
#include "map/MapGenerator.hpp"
#include "map/MapModification.hpp"
#include <shared_mutex>

class MapManager {
public:
    MapManager() {
        MapGenerator generator(MapState::MAP_HEIGHT, MapState::MAP_WIDTH);
        initializeMap(generator.getMap());
    }

    const MapState& getMap() const {
        std::shared_lock lock(mapMutex);
        return currentMap;
    }

    std::vector<SpawnPoint> getSpawnPoints() const {
        std::shared_lock lock(mapMutex);
        return {currentMap.spawnPoints.begin(), currentMap.spawnPoints.end()};
    }

    std::string getSerializedMapState() const {
        std::shared_lock lock(mapMutex);
        if (cachedSerializedMap.empty()) {
            cachedSerializedMap = nlohmann::json(currentMap).dump();
        }
        return cachedSerializedMap;
    }

    bool tryModifyTile(const MapModification& mod) {
        std::unique_lock lock(mapMutex);

        if (!currentMap.isValidPosition(mod.x, mod.y)) {
            return false;
        }

        auto& tile = currentMap.at(mod.x, mod.y);

        // Handle indestructible walls
        if (tile.type == TileType::IndestructibleWall) {
            return false;
        }

        // Handle destructible walls
        if (tile.type == TileType::DestructibleWall) {
            currentMap.destroyedWalls++;
            tile.type = TileType::Path; // Change the wall to a path
        }

        // Handle tile reveal
        if (mod.reveal && !tile.revealed) {
            tile.revealed = true;
            if (tile.type == TileType::HiddenBomb) {
                currentMap.revealedBombs++;
            }
        }

        // Apply new tile type using TileType directly
        tile.type = static_cast<TileType>(mod.newType);

        tile.updateProperties();
        cachedSerializedMap.clear(); // Invalidate the serialized map cache

        return true;
    }

    bool isCollision(uint32_t x, uint32_t y) const {
        std::shared_lock lock(mapMutex);
        return !currentMap.isWalkable(x, y);
    }

private:
    void initializeMap(const std::vector<std::vector<int>>& matrix) {
        for (size_t y = 0; y < matrix.size(); ++y) {
            for (size_t x = 0; x < matrix[y].size(); ++x) {
                currentMap.tiles[y][x] = MapTile(static_cast<TileType>(matrix[y][x]));
            }
        }
    }

    mutable std::shared_mutex mapMutex;
    mutable std::string cachedSerializedMap;
    MapState currentMap;
};
