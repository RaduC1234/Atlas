#pragma once

#include "Player.hpp"
#include <SQLiteCpp/SQLiteCpp.h>

/**
 * Service class for user data
 */
class PlayerService {
public:

    explicit PlayerService(const std::string& databasePath);

    ~PlayerService() = default;

    /**
     * Adds an user to the database
     * @param user The user to add
     */
    void add(const Player& user);

    /**
     * Gets an user by its username
     * @param username The username of the user
     * @return The user with the given username
     */
    void update(const Player& user);

    /**
     * Gets an user by its username
     * @param username The username of the user
     * @return The user with the given username
     */
    void deleteByUsername(const std::string& username);

    /**
     * Returns if the user with the given username exists in the database
     */
    bool exists(const std::string& username);

private:
    SQLite::Database db;
};
