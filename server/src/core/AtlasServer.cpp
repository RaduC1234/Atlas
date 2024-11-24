#include "AtlasServer.hpp"

#include <crow.h>

#include "ServerNetworkManager.hpp"


void AtlasServer::run() {

    Log::init();
    AT_INFO("Server warming up...");

    FileSystem::setWorkingDirectory(ATLAS_WORKING_DIRECTORY);
    AT_INFO("Working directory is: {0}.", ATLAS_WORKING_DIRECTORY);

    AT_TRACE("Attempting to read config file...");
    TRY_CATCH(this->serverConfig = Config::build("server.config"), AT_FATAL("Error reading config file. Exiting..."););
    AT_INFO("Config file read successfully");

    const double ticksPerSec = this->serverConfig["server_ticks_per_sec"].toDouble();
    const double millisecondsPerTick = 1000 / ticksPerSec;

    AT_INFO("Starting Atlas Dedicated Server on port {0}. Running at {1} ticks per second.", this->serverConfig["server_port"].toString(), ticksPerSec);
    Time nextLoop = Time::now();

    ServerNetworkManager::start(8080);

    while(isRunning) {
        while (nextLoop < Time::now()) {
            ServerNetworkManager::tick();
            nextLoop  = nextLoop.addMilliseconds(millisecondsPerTick);

            if(nextLoop > Time::now()) {
                std::this_thread::sleep_for(nextLoop - Time::now());
            }
        }
    }
}
