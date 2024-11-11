cmake_minimum_required(VERSION 3.10)
project(engine)

file(GLOB_RECURSE ENGINE_SOURCES "src/*.cpp" "src/*.hpp")

add_library(engine OBJECT ${ENGINE_SOURCES}
        src/UI/TextBox.hpp)

target_compile_definitions(engine PUBLIC ATLAS_ENABLE_ASSERT)

# SQLite
find_package(SQLiteCpp CONFIG REQUIRED)
target_link_libraries(engine PUBLIC SQLiteCpp)

# EnTT
find_package(EnTT CONFIG REQUIRED)
target_link_libraries(engine PUBLIC EnTT::EnTT)

# GLM
find_package(glm CONFIG REQUIRED)
target_link_libraries(engine PUBLIC glm::glm-header-only)

# OpenGL
add_subdirectory(dependencies/glad)
target_link_libraries(engine PUBLIC glad)

# FreeType
find_package(freetype CONFIG REQUIRED)
target_link_libraries(engine PUBLIC freetype)

# GLFW3
find_package(glfw3 CONFIG REQUIRED)
target_link_libraries(engine PUBLIC glfw)

# stb
target_include_directories(engine PRIVATE ${VCPKG_INCLUDE_DIR})

# Json
find_package(nlohmann_json CONFIG REQUIRED)
target_link_libraries(engine PUBLIC nlohmann_json::nlohmann_json)

# Crow
find_package(Crow CONFIG REQUIRED)
target_link_libraries(engine PUBLIC Crow::Crow)

target_include_directories(engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
