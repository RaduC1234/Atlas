cmake_minimum_required(VERSION 3.10)
project(engine)

# Gather source files
file(GLOB_RECURSE ENGINE_SOURCES "src/*.cpp" "src/*.hpp")

# Create engine library
add_library(engine OBJECT ${ENGINE_SOURCES})

# Define public compile definitions
target_compile_definitions(engine PUBLIC ATLAS_ENABLE_ASSERT ATLAS_BACKEND_SFML)

#SFML
find_package(SFML COMPONENTS system window graphics CONFIG REQUIRED)
target_link_libraries(engine PUBLIC sfml-system sfml-network sfml-graphics sfml-window)

# SQLite
find_package(SQLiteCpp CONFIG REQUIRED)
target_link_libraries(engine PUBLIC SQLiteCpp)

# EnTT
find_package(EnTT CONFIG REQUIRED)
target_link_libraries(engine PUBLIC EnTT::EnTT)

# GLM
find_package(glm CONFIG REQUIRED)
target_link_libraries(engine PUBLIC glm::glm-header-only)

# Add engine source directory to include paths
target_include_directories(engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
