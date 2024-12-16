#pragma once

#include <Atlas.hpp>

#include "data/Player.hpp"
#include "data/Match.hpp"

class Lobby {
public:
    void startGame() {

    }

private:

    static uint32_t generateId() {
        std::random_device rd;
        std::mt19937 gen(rd);
        std::uniform_int_distribution<> dis(1000, 9999);
        return dis(gen);
    }

    Registry levelRegistry; // entt registry
    std::array<uint64_t, 4> clients = { 0, 0, 0, 0};
    uint32_t id{generateId()};
};
