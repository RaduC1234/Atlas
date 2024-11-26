cmake_minimum_required(VERSION 3.28)
project(Client)

file(GLOB_RECURSE SERVER_SOURCES "src/*.cpp" "src/*.hpp")

add_executable(client ${SERVER_SOURCES})

target_link_libraries(client PRIVATE engine)

target_include_directories(client PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

target_compile_features(client PRIVATE cxx_std_20)
