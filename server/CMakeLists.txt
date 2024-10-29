cmake_minimum_required(VERSION 3.10)
project(server)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SERVER_SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/src/ServerMain.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/core/ServerNetworkManager.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/core/ServerNetworkManager.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/core/Lobby.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/core/Lobby.hpp"
        src/core/Log.cpp
        src/core/Log.hpp
        src/core/DatabaseManager.cpp
        src/core/DatabaseManager.hpp
)

add_executable(server ${SERVER_SOURCES})

target_include_directories(server PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/core"
)

# Find SFML
find_package(SFML 2.6.1 COMPONENTS network system graphics window REQUIRED)

# Remove the static/dynamic option since we'll use what vcpkg provides
target_link_libraries(server PUBLIC
        engine
        sfml-network
        sfml-system
        sfml-graphics
        sfml-window
)