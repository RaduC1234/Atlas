cmake_minimum_required(VERSION 3.10)
project(server)

file(GLOB_RECURSE SERVER_SOURCES "src/*.cpp" "src/*.hpp")

add_executable(server ${SERVER_SOURCES})

target_include_directories(server PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

target_link_libraries(server PRIVATE engine)

#SQL
find_package(SqliteOrm REQUIRED)
target_link_libraries(server PUBLIC sqlite_orm::sqlite_orm)

# Crow
find_package(Crow CONFIG REQUIRED)
target_link_libraries(server PUBLIC Crow::Crow)
