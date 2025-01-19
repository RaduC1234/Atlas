cmake_minimum_required(VERSION 3.28)
project(Client)

file(GLOB_RECURSE SERVER_SOURCES "src/*.cpp" "src/*.hpp")

add_executable(client ${SERVER_SOURCES})

target_link_libraries(client PRIVATE engine)

target_include_directories(client PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

target_compile_features(client PRIVATE cxx_std_20)

# OpenGL
add_subdirectory(../dependencies/glad ${CMAKE_BINARY_DIR}/glad)
target_link_libraries(client PUBLIC glad)

# GLFW
find_package(glfw3 CONFIG REQUIRED)
target_link_libraries(client PUBLIC glfw)

# FreeType
find_package(freetype CONFIG REQUIRED)
target_link_libraries(client PUBLIC freetype)

# ImGui
add_library(imgui STATIC
        ${CMAKE_CURRENT_SOURCE_DIR}/../dependencies/imgui/imgui.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../dependencies/imgui/imgui_draw.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../dependencies/imgui/imgui_widgets.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../dependencies/imgui/imgui_demo.cpp   # Optional
        ${CMAKE_CURRENT_SOURCE_DIR}/../dependencies/imgui/imgui_tables.cpp # Add this line
        ${CMAKE_CURRENT_SOURCE_DIR}/../dependencies/imgui/backends/imgui_impl_glfw.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../dependencies/imgui/backends/imgui_impl_opengl3.cpp
)

target_include_directories(imgui PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/../dependencies/imgui
        ${CMAKE_CURRENT_SOURCE_DIR}/../dependencies/imgui/backends
)

target_link_libraries(imgui PUBLIC glfw glad)
target_link_libraries(client PUBLIC imgui)

# stb
target_include_directories(client PUBLIC ${VCPKG_INCLUDE_DIR})

# Boost
find_package(Boost REQUIRED)
target_link_libraries(client PUBLIC Boost::boost)

# Cpr
add_subdirectory(../dependencies/cpr ${CMAKE_BINARY_DIR}/cpr)
target_link_libraries(client PUBLIC cpr::cpr)

add_custom_command(
        TARGET client POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_BINARY_DIR}/cpr/cpr/cpr.dll" # Path to cpr.dll in the build directory
        "${CMAKE_CURRENT_BINARY_DIR}" # Output directory for the client executable
        COMMENT "Copying cpr.dll to client build directory"
)

add_custom_command(
        TARGET client POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_BINARY_DIR}/_deps/curl-build/lib/libcurl-d.dll"
        "${CMAKE_CURRENT_BINARY_DIR}"
        COMMENT "Copying libcurl-d.dll to client build directory"
)
