#pragma once

#include "PlayerService.hpp"

class DatabaseManager {
public:

    /**
     * Initialize the database manager.
     * @param databasePath the relative the path of the SQLite database file.
     */
    static inline void init(const std::string& databasePath) {
        playerService = new PlayerService(databasePath);
    }

    /**
     * Get the player service.
     * @return the player service.
     */
    static inline PlayerService* getPlayerService() {
        return playerService;
    }


    /**
     * Shutdown the database manager.
     */
    static inline void shutdown() {
        delete playerService;
    }

private:
    inline static PlayerService* playerService;
};
