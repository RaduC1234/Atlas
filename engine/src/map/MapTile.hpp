#pragma once

#include <Atlas.hpp>

enum class TileType : uint8_t {
    Path = 0,
    Grass = 1,
    Bush = 2,
    DestructibleWall = 3,
    IndestructibleWall = 4,
    HealthPickup = 5,
    SpeedBoost = 6,
    HiddenBomb = 7
};

struct MapTile {
    TileType type{TileType::Path};
    bool revealed{true};
    bool damaged{false};  // For the breakable walls
    bool walkable{true};  // collision flag

    MapTile(TileType t = TileType::Path) : type(t) {
        updateProperties();
    }

    void updateProperties() {
        walkable = (type == TileType::Path || type == TileType::Grass ||
                   type == TileType::HealthPickup || type == TileType::SpeedBoost);
    }

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(MapTile, type, revealed, damaged, walkable)
};