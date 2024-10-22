cmake_minimum_required(VERSION 3.29)
project(Atlas)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_subdirectory(engine)
add_subdirectory(client)
add_subdirectory(server)
