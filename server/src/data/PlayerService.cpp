#include "PlayerService.hpp"

PlayerService::PlayerService(const std::string &databasePath) : db(databasePath, SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE) {
    db.exec("CREATE TABLE IF NOT EXISTS Player ("
            "username TEXT PRIMARY KEY, "
            "password TEXT, "
            "mmr INTEGER DEFAULT 0, "
            "profilePicture INTEGER);");
}

void PlayerService::add(const Player &user) {
    try {
        if (exists(user.getUsername())) {
            throw std::runtime_error("User with the given username already exists in the database");
        }

        SQLite::Statement query(db, "INSERT INTO Player (username, password, mmr, profilePicture) VALUES (?, ?, ?, ?)");

        query.bind(1, user.getUsername());
        query.bind(2, user.getPassword());
        query.bind(3, user.getMmr());
        query.bind(4, user.getProfilePicture());

        query.exec();  // Execute the query
    } catch (const std::exception &exception) {
        AT_ERROR("Error adding user \'{0}\'. {1}.", user.getUsername(), exception.what());
    }
}

void PlayerService::update(const Player &user) {
    try {
        if (!exists(user.getUsername())) {
            throw std::runtime_error("User with the given username does not exist in the database");
        }

        SQLite::Statement query(db, "UPDATE Player SET password = ?, mmr = ?, profilePicture = ? WHERE username = ?");

        query.bind(1, user.getPassword());
        query.bind(2, user.getMmr());
        query.bind(3, user.getProfilePicture());
        query.bind(4, user.getUsername());

        query.exec();
    } catch (const std::exception &exception) {
        AT_ERROR("Error updating user \'{0}\'. {1}.", user.getUsername(), exception.what());
    }
}

void PlayerService::deleteByUsername(const std::string &username) {

    try {
        if (!exists(username)) {
            throw std::runtime_error("User with the given username does not exist in the database");
        }

        SQLite::Statement query(db, "DELETE FROM Player WHERE username = ?");
        query.bind(1, username);

        query.exec();
    } catch (const std::exception& exception) {
        AT_ERROR("Error deleting user \'{0}\'. {1}.", username, exception.what());
    }
}

bool PlayerService::exists(const std::string &username) {
    SQLite::Statement query(db, "SELECT COUNT(*) FROM Player WHERE username = ?");
    query.bind(1, username);

    return query.executeStep() && query.getColumn(0).getInt() > 0;
}
