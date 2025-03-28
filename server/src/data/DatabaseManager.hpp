#pragma once

#include <sqlite_orm/sqlite_orm.h>
#include "Player.hpp"
#include "Match.hpp"

namespace sql= sqlite_orm ;

inline auto createStorage(const std::string &dbFile) {
    return sql::make_storage(
        dbFile,
        sql::make_table(
            "Players",
            sql::make_column("id", &Player::getId, &Player::setId, sql::primary_key().autoincrement()),
            sql::make_column("username", &Player::getUsername, &Player::setUsername),
            sql::make_column("password", &Player::getPassword, &Player::setPassword),
            sql::make_column("mmr", &Player::getMmr, &Player::setMmr),
            sql::make_column("glickoRating", &Player::getGlickoRating, &Player::setGlickoRating),
            sql::make_column("ratingDeviation", &Player::getRatingDeviation, &Player::setRatingDeviation),
            sql::make_column("volatility", &Player::getVolatility, &Player::setVolatility),
            sql::make_column("matchId", &Player::getMatchId, &Player::setMatchId)

        ),
        sql::make_table(
            "Matches",
            sql::make_column("id", &Match::getId, &Match::setId, sql::primary_key().autoincrement()),
            sql::make_column("timestamp", &Match::getTimestamp, &Match::setTimestamp),
            sql::make_column("glickoMatchWeight", &Match::getGlickoMatchWeight, &Match::setGlickoMatchWeight),
            sql::make_column("winnerId", &Match::getWinnerId, &Match::setWinnerId)
        )
    );
}

using Storage = decltype(createStorage(""));

template <typename T>
concept DatabaseType = std::same_as<T, Player> || std::same_as<T, Match>;

class DatabaseManager {
public:
    static void init(const std::string &dbFile) {
        m_storage = new Storage(createStorage(dbFile));
        m_storage->sync_schema();
    }

    static void shutdown() {
        delete m_storage;
    }

    template <DatabaseType T>
    static void create(const T &entity) {
        m_storage->insert(entity);
    }

    template <DatabaseType T, typename... Args>
    static T emplace_create(Args&&... args) {
        T entity(std::forward<Args>(args)...);
        m_storage->insert(entity);
        return entity;
    }

    template <DatabaseType T>
    static std::vector<T> getAll() {
        return m_storage->get_all<T>();
    }

    template <DatabaseType T>
    static void update(const T &entity) {
        m_storage->update(entity);
    }

    template <DatabaseType T>
    static void remove(const T &entity) {
        m_storage->remove<T>(entity.getId());
    }

private:
    inline static Storage* m_storage = nullptr;
};
