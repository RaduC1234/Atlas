#pragma once

#include <Atlas.hpp>

class AtlasServer {
public:
    void run();
private:
    Config serverConfig;
    std::atomic_bool isRunning{true};
};
