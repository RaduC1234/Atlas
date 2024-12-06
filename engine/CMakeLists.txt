cmake_minimum_required(VERSION 3.10)
project(engine)

# Set C++ Standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Collect source files
file(GLOB_RECURSE ENGINE_SOURCES "src/*.cpp" "src/*.hpp")

# Add the engine as a library (OBJECT for flexibility, STATIC for fully linked binaries)
add_library(engine OBJECT ${ENGINE_SOURCES})

# Set compilation definitions
target_compile_definitions(engine PUBLIC ATLAS_ENABLE_ASSERT)

# Set include directories for the engine
target_include_directories(engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

# ===========================
# Dependencies and Libraries
# ===========================

find_package(SqliteOrm REQUIRED)
target_link_libraries(engine PUBLIC sqlite_orm::sqlite_orm)

# EnTT
find_package(EnTT CONFIG REQUIRED)
target_link_libraries(engine PUBLIC EnTT::EnTT)

# GLM (header-only library)
find_package(glm CONFIG REQUIRED)
target_link_libraries(engine PUBLIC glm::glm-header-only)

# OpenGL and GLFW
add_subdirectory(../dependencies/glad ${CMAKE_BINARY_DIR}/glad)
find_package(glfw3 CONFIG REQUIRED)
target_link_libraries(engine PUBLIC glad glfw)

# FreeType
find_package(freetype CONFIG REQUIRED)
target_link_libraries(engine PUBLIC freetype)

# stb (assume stb headers are included in vcpkg include directory)
target_include_directories(engine PRIVATE ${VCPKG_INCLUDE_DIR})

# Json
find_package(nlohmann_json CONFIG REQUIRED)
target_link_libraries(engine PUBLIC nlohmann_json::nlohmann_json)

# Crow
find_package(Crow CONFIG REQUIRED)
target_link_libraries(engine PUBLIC Crow::Crow)

add_subdirectory(../dependencies/cpr ${CMAKE_BINARY_DIR}/cpr)
target_link_libraries(engine PUBLIC cpr::cpr)

#[[# CPR (Curl-based HTTP library)
include(FetchContent)
FetchContent_Declare(cpr GIT_REPOSITORY https://github.com/libcpr/cpr.git) # Replace with your desired git commit from: https://github.com/libcpr/cpr/releases
FetchContent_MakeAvailable(cpr)

target_link_libraries(engine PUBLIC cpr::cpr)]]

# ===========================
# ImGui Configuration
# ===========================

# Add ImGui as a STATIC library with source files
add_library(imgui STATIC
        ${CMAKE_CURRENT_SOURCE_DIR}/../dependencies/imgui/imgui.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../dependencies/imgui/imgui_draw.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../dependencies/imgui/imgui_widgets.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../dependencies/imgui/imgui_demo.cpp   # Optional
        ${CMAKE_CURRENT_SOURCE_DIR}/../dependencies/imgui/imgui_tables.cpp # Add this line
        ${CMAKE_CURRENT_SOURCE_DIR}/../dependencies/imgui/backends/imgui_impl_glfw.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../dependencies/imgui/backends/imgui_impl_opengl3.cpp
)

# Add the necessary include directories for ImGui
target_include_directories(imgui PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/../dependencies/imgui
        ${CMAKE_CURRENT_SOURCE_DIR}/../dependencies/imgui/backends
)

# Link ImGui with glfw and glad
target_link_libraries(imgui PUBLIC glfw glad)

# Link ImGui to engine
target_link_libraries(engine PUBLIC imgui)