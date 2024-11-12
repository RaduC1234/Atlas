#include "Atlas.hpp"
#include "core/Server.hpp"
#include <iostream>

int main() {
    try {
        Server server("127.0.0.1", 8080);
        server.start();
    } catch (const std::exception& e) {
        std::cerr << "Server error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
