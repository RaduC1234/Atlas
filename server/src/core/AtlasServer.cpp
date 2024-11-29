#include "AtlasServer.hpp"

#include <crow.h>

#include "ServerNetworkService.hpp"


void AtlasServer::run() {

    Log::init();
    AT_INFO("Server Atlas Server");

    FileSystem::setWorkingDirectory(ATLAS_WORKING_DIRECTORY);
    AT_INFO("Working directory is: {0}.", ATLAS_WORKING_DIRECTORY);

    AT_TRACE("Attempting to read config file...");
    TRY_CATCH(this->serverConfig = Config::build("server.config"), AT_FATAL("Error reading config file. Exiting..."););
    AT_INFO("Config file read successfully");

    const double ticksPerSec = this->serverConfig["server_ticks_per_sec"].toDouble();
    const double millisecondsPerTick = 1000 / ticksPerSec;

    AT_INFO("Starting Atlas Dedicated Server on port {0}. Running at {1} ticks per second.", this->serverConfig["server_port"].toString(), ticksPerSec);
    Time nextLoop = Time::now();

    this->serverManager.start(this->serverConfig["server_port"].toInt());

    AT_INFO("Server finished loading...");

    while (isRunning) {
        while (nextLoop < Time::now()) {

            /*// Check for server lag
            Time currentTime = Time::now();
            if (currentTime - nextLoop > std::chrono::milliseconds(millisecondsPerTick)) {
                double lagSeconds = (currentTime - nextLoop).count() / 1000.0;
                AT_WARN("Server is running behind! Lagging by {0:.2f} seconds.", lagSeconds);
            }*/


            //this->serverManager.tick(); // Process tick
            nextLoop = nextLoop.addMilliseconds(millisecondsPerTick); // Schedule next tick

            if (nextLoop > Time::now()) {  // Sleep if ahead of schedule
                std::this_thread::sleep_for(nextLoop - Time::now());
            }
        }
    }

}
