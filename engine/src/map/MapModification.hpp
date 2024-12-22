#pragma once
#include <map/MapTile.hpp>
struct MapModification {
    uint32_t x, y;
    TileType newType;
    bool reveal{false};
    uint32_t playerId{0};
    
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(MapModification, x, y, newType, reveal, playerId)
};