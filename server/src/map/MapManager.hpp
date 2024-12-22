#pragma once
#include "MapGenerator.hpp"
#include "map/MapModification.hpp"
#include "map/MapState.hpp"
#include "map/MapTile.hpp"

class MapManager {
public:
    MapManager() {
        currentMap = MapGenerator::generateMap();
    }
    
    const MapState& getMap() const { 
        return currentMap; 
    }
    
    bool tryModifyTile(const MapModification& mod) {
        if (!currentMap.isValidPosition(mod.x, mod.y)) {
            return false;
        }
        
        auto& tile = currentMap.at(mod.x, mod.y);
        
        // Regulile pentru modificare
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
    
    std::optional<SpawnPoint> getNextSpawnPoint() {
        for (auto& spawn : currentMap.spawnPoints) {
            if (!spawn.occupied) {
                spawn.occupied = true;
                return spawn;
            }
        }
        return std::nullopt;
    }
    
    void releaseSpawnPoint(const glm::vec2& position) {
        for (auto& spawn : currentMap.spawnPoints) {
            if (spawn.position == position) {
                spawn.occupied = false;
                break;
            }
        }
    }
    
    bool isWalkable(uint32_t x, uint32_t y) const {
        return currentMap.isWalkable(x, y);
    }

private:
    MapState currentMap;
};
