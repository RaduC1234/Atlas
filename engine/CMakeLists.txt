cmake_minimum_required(VERSION 3.10)
project(Engine)

file(GLOB_RECURSE ENGINE_SOURCES "src/*.cpp" "src/*.hpp")

add_library(engine OBJECT ${ENGINE_SOURCES})

target_include_directories(engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

target_compile_definitions(engine PUBLIC ATLAS_ENABLE_ASSERT)
