#pragma once
#include <Atlas.hpp>
#include <map/MapTile.hpp>
#include <map/MapState.hpp>

struct SpawnPoint {
    glm::vec2 position;     // Map position on the space
    float rotation;         // rotation
    bool occupied{false};   // occupied by player

    SpawnPoint(glm::vec2 pos = glm::vec2(0.0f), float rot = 0.0f)
        : position(pos), rotation(rot) {}

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(SpawnPoint, position, rotation, occupied)
};

struct MapState {
    static constexpr uint32_t MAP_WIDTH = 52;
    static constexpr uint32_t MAP_HEIGHT = 60;
    static constexpr float TILE_SIZE = 100.0f;  // Changed to 100 pixels

    std::vector<std::vector<MapTile>> tiles;
    std::array<SpawnPoint, 4> spawnPoints;
    uint32_t destroyedWalls{0};
    uint32_t revealedBombs{0};

    MapState() {
        initializeMap();
        initializeSpawnPoints();
    }

    void initializeMap() {
        tiles.resize(MAP_HEIGHT, std::vector<MapTile>(MAP_WIDTH));
    }

    void initializeSpawnPoints() {
        // offset
        spawnPoints = {
            SpawnPoint({2.0f, 2.0f}, 45.0f),          // left up
            SpawnPoint({MAP_WIDTH-3.0f, 2.0f}, 135.0f),      // right up
            SpawnPoint({2.0f, MAP_HEIGHT-3.0f}, 315.0f),     // left down
            SpawnPoint({MAP_WIDTH-3.0f, MAP_HEIGHT-3.0f}, 225.0f)  // right down
        };
    }

    MapTile& at(uint32_t x, uint32_t y) {
        return tiles[y][x];
    }

    const MapTile& at(uint32_t x, uint32_t y) const {
        return tiles[y][x];
    }

    bool isValidPosition(uint32_t x, uint32_t y) const {
        return x < MAP_WIDTH && y < MAP_HEIGHT;
    }
    
    bool isWalkable(uint32_t x, uint32_t y) const {
        return isValidPosition(x, y) && tiles[y][x].walkable;
    }
    
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(MapState, tiles, spawnPoints, 
                                  destroyedWalls, revealedBombs)
};