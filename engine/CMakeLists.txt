cmake_minimum_required(VERSION 3.10)
project(engine)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE ENGINE_SOURCES "src/*.cpp" "src/*.hpp")

add_library(engine STATIC ${ENGINE_SOURCES})

target_compile_definitions(engine PUBLIC ATLAS_ENABLE_ASSERT)

target_include_directories(engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

# EnTT
find_package(EnTT CONFIG REQUIRED)
target_link_libraries(engine PUBLIC EnTT::EnTT)

# GLM (header-only library)
find_package(glm CONFIG REQUIRED)
target_link_libraries(engine PUBLIC glm::glm-header-only)

# Json
find_package(nlohmann_json CONFIG REQUIRED)
target_link_libraries(engine PUBLIC nlohmann_json::nlohmann_json)

