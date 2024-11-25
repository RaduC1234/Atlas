#pragma once

#include <Atlas.hpp>

#include "data/Match.hpp"

struct GameMap {
    //std::
};
class Lobby {
public:
    void setup();
    void start();
private:
    std::vector<Client> clients;
    uint32_t id{getId()};

    auto generateMap()

    uint32_t getId() {
        static uint32_t ids = 0;
        return ids++;
    }
};

inline void Lobby::setup() {
    //GameMap<12, 14> gameMap = ;
}

inline void Lobby::start() {
}
