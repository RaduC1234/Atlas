#include "AtlasServer.hpp"

#include "data/DatabaseManager.hpp"


void AtlasServer::run() {
    Log::init();
    AT_INFO("Server Atlas Server");

    FileSystem::setWorkingDirectory(ATLAS_WORKING_DIRECTORY);
    AT_INFO("Working directory is: {0}.", ATLAS_WORKING_DIRECTORY);

    AT_TRACE("Attempting to read config file...");
    TRY_CATCH(this->serverConfig = Config::build("server.config"), AT_FATAL("Error reading config file. Exiting..."););
    AT_INFO("Config file read successfully");

    AT_TRACE("Attempting to create database file...");
    TRY_CATCH(
        {
        auto dataPath = this->serverConfig["database_path"].toString();
        DatabaseManager::init(dataPath);
        AT_INFO("Database filepath is {0}.", dataPath);
        },
        {
        AT_ERROR("Failed to load database: {0}", e.what());
        }
    );

    const double ticksPerSec = this->serverConfig["server_ticks_per_sec"].toDouble();
    const double millisecondsPerTick = 1000 / ticksPerSec;

    AT_INFO("Starting Atlas Dedicated Server on port {0}. Running at {1} ticks per second.", this->serverConfig["server_port"].toString(), ticksPerSec);

    this->serverManager.start(this->serverConfig["server_port"].toInt());

    AT_INFO("Server finished loading...");
}
