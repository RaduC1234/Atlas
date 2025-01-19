#include "AtlasServer.hpp"

#include "data/DatabaseManager.hpp"


void AtlasServer::run() {
    Log::init();
    AT_INFO("Server Atlas Server");

    FileSystem::setWorkingDirectory(ATLAS_WORKING_DIRECTORY);
    AT_INFO("Working directory is: {0}.", ATLAS_WORKING_DIRECTORY);

    AT_TRACE("Attempting to read config file...");

    try {
        this->serverConfig = Config::build("server.config");
    } catch (const std::exception &e) {
        AT_ERROR("Error reading config file. Exiting...");
    }

    AT_INFO("Config file read successfully");

    AT_TRACE("Attempting to create database file...");

    try {
        auto dataPath = this->serverConfig["database_path"].toString();
        DatabaseManager::init(dataPath);
        AT_INFO("Database filepath is {0}.", dataPath);
    } catch (const std::exception &e) {
        AT_ERROR("Failed to load database: {0}", e.what());
    };

    AT_INFO("Starting Atlas Dedicated Server on port {0}.", this->serverConfig["server_port"].toString());

    this->serverManager.start(this->serverConfig["server_port"].toInt());

    AT_INFO("Server finished loading...");
}
