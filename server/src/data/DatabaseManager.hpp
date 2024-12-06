#pragma once

#include <Atlas.hpp>

#include <sqlite_orm/sqlite_orm.h>
#include "Player.hpp"
#include "Match.hpp"

using namespace sqlite_orm;

auto createStorage(const std::string& path) {
    return make_storage(path,
        make_table("Player",
            make_column("username", &Player::getUsername, primary_key()),
            make_column("password", &Player::getPassword),
            make_column("mmr", &Player::getMmr),
            make_column("profilePicture", &Player::getProfilePicture),
            make_column("glickoRating", &Player::getGlickoRating),
            make_column("ratingDeviation", &Player::getRatingDeviation),
            make_column("volatility", &Player::getVolatility)
        ),
        make_table("Match",
            make_column("id", &Match::getId, autoincrement(), primary_key()),
            make_column("timestamp", &Match::getTimestamp),
            make_column("winnerUsername", &Match::getWinnerUsername),
            make_column("glickoMatchWeight", &Match::getGlickoMatchWeight)
        ),
        make_table("MatchPlayer",
            make_column("matchId", &Match::getId),
            make_column("playerUsername", &Player::getUsername),
            primary_key(&Match::getId, &Player::getUsername)
        )
    );
}

template<typename T>
class DatabaseService {
public:
    explicit DatabaseService(const std::string& databasePath) : storage(createStorage(databasePath)) {
        storage.sync_schema();
    }

    void add(const T& item) {
        storage.replace(item);
    }

    void update(const T& item) {
        storage.update(item);
    }

    void remove(const std::string& primaryKey) {
        storage.remove<T>(primaryKey);
    }

    std::vector<T> getAll() {
        return storage.get_all<T>();
    }

    T get(const std::string& primaryKey) {
        return storage.get<T>(primaryKey);
    }

    bool exists(const std::string& primaryKey) {
        return storage.count<T>(where(c(&T::getUsername) == primaryKey)) > 0;
    }

private:
    decltype(createStorage("")) storage;
};

/*int main() {
    const std::string db_file = "game_data.sqlite";

    DatabaseService<Player> playerService(db_file);

    Player player1("player1", "password1", 1200, 1, 1500.0, 300.0, 0.06);
    playerService.add(player1);

    if (playerService.exists("player1")) {
        std::cout << "Player1 exists!\n";
    }

    Player retrievedPlayer = playerService.get("player1");
    std::cout << "Retrieved Player: " << retrievedPlayer.getUsername() << '\n';

    retrievedPlayer.setMmr(1300);
    playerService.update(retrievedPlayer);

    auto players = playerService.getAll();
    std::cout << "Total Players: " << players.size() << '\n';

    playerService.remove("player1");

    return 0;
}#1#
*/
