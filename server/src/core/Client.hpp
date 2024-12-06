#pragma once
#include <crow/websocket.h>


class Client {
public:
    explicit Client(crow::websocket::connection *connection)
        : connection(connection) {
    }

    bool isAuthenticated() {
        return this->authenticated;
    }

    crow::websocket::connection *getConnection() const {
        return connection;
    }

    bool operator<(const Client &other) const {
        return connection < other.connection;
    }

    bool operator==(const Client &other) const {
        return this->connection == other.connection;
    }

private:
    bool authenticated{false};
    crow::websocket::connection * connection{nullptr};
};
