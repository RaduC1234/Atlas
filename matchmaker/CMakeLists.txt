cmake_minimum_required(VERSION 3.10)
project(matchmaker)

# Glob source files
file(GLOB_RECURSE SERVER_SOURCES "src/*.cpp" "src/*.hpp")

# Define the shared library
add_library(matchmaker SHARED ${SERVER_SOURCES})

# Include directories
target_include_directories(matchmaker PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

# Link against the engine library
target_link_libraries(matchmaker PRIVATE engine)

# Define the preprocessor directive
target_compile_definitions(matchmaker PUBLIC ATLAS_BUILD_SHARED)
target_compile_definitions(matchmaker PRIVATE MATCHMAKER_EXPORTS)

# Set the server build directory dynamically
set(SERVER_BUILD_DIR "${CMAKE_BINARY_DIR}/server")

# Ensure the DLL is copied to the server build directory after build
add_custom_command(
        TARGET matchmaker POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory ${SERVER_BUILD_DIR}
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:matchmaker> ${SERVER_BUILD_DIR}
        COMMENT "Copying matchmaker DLL to server build directory: ${SERVER_BUILD_DIR}"
)