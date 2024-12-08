#pragma once

#include <Atlas.hpp>
#include <crow.h>

#include "network/ServerNetworkService.hpp"

class AtlasServer {
public:
    void run();
private:
    Config serverConfig;
    std::atomic_bool isRunning{true};
    ServerNetworkService serverManager;
};
