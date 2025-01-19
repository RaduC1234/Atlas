#pragma once

#include <Atlas.hpp>
#include "map/MapState.hpp"
#include "map/MapTile.hpp"
#include "MapGenerator.hpp"
#include "map/MapModification.hpp"

class MapManager {
public:
    MapManager() {
        MapGenerator generator(MapState::MAP_HEIGHT, MapState::MAP_WIDTH);
        const auto& matrix = generator.getMap();
        currentMap = MapState();

        for (size_t y = 0; y < matrix.size(); ++y) {
            for (size_t x = 0; x < matrix[y].size(); ++x) {
                currentMap.tiles[y][x] = MapTile(static_cast<TileType>(matrix[y][x]));
            }
        }
    }

    const MapState& getMap() const {
        return currentMap;
    }

    std::vector<SpawnPoint> getSpawnPoints() const {
        return {currentMap.spawnPoints.begin(), currentMap.spawnPoints.end()};
    }

    std::string getSerializedMapState() const {
        return nlohmann::json(currentMap).dump(); // Serialize the entire `MapState` object
    }

    bool tryModifyTile(const MapModification& mod) {
        if (!currentMap.isValidPosition(mod.x, mod.y)) {
            return false;
        }

        auto& tile = currentMap.at(mod.x, mod.y);

        if (tile.type == TileType::IndestructibleWall) {
            return false;
        }

        if (tile.type == TileType::DestructibleWall) {
            currentMap.destroyedWalls++;
        }

        if (mod.reveal && !tile.revealed) {
            tile.revealed = true;
            if (tile.type == TileType::HiddenBomb) {
                currentMap.revealedBombs++;
            }
        }

        tile.type = mod.newType;
        tile.updateProperties();

        return true;
    }

    bool isCollision(uint32_t x, uint32_t y) const {
        return !currentMap.isWalkable(x, y);
    }

private:
    MapState currentMap;
};