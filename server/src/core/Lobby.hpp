#pragma once

#include <Atlas.hpp>
#include "Client.hpp"

#include "data/Match.hpp"

class Lobby {
public:

private:


    static uint32_t generateId() {
        std::random_device rd;
        std::mt19937 gen(rd);
        std::uniform_int_distribution<> dis(1000, 9999);
        return dis(gen);
    }

    Registry levelRegistry; // entt registry
    std::vector<Client> clients;
    uint32_t id{generateId()};
};
