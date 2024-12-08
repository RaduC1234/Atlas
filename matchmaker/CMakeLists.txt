cmake_minimum_required(VERSION 3.10)
project(matchmaker)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Print current directory information for debugging
message(STATUS "CMAKE_CURRENT_SOURCE_DIR: ${CMAKE_CURRENT_SOURCE_DIR}")
message(STATUS "PROJECT_SOURCE_DIR: ${PROJECT_SOURCE_DIR}")

# Define source files with src/ prefix
set(SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/src/dllmain.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/Glicko2.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/MatchmakerMain.cpp"
)

# Define header files with src/ prefix
set(HEADERS
        "${CMAKE_CURRENT_SOURCE_DIR}/src/Glicko2.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/LobbyMatchQuality.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/Matchmaking.hpp"
)

# Print the source files for verification
message(STATUS "Source files:")
foreach(source ${SOURCES})
    message(STATUS "  ${source}")
endforeach()

# Define the shared library
add_library(matchmaker SHARED ${SOURCES} ${HEADERS})

# Include src directory for header files
target_include_directories(matchmaker PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src")

# Define the preprocessor directive
target_compile_definitions(matchmaker PUBLIC ATLAS_BUILD_SHARED)
target_compile_definitions(matchmaker PRIVATE MATCHMAKER_EXPORTS)

# Set the output directory for the DLL
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR})

# Create the executable
add_executable(matchmaking "${CMAKE_CURRENT_SOURCE_DIR}/src/MatchmakerMain.cpp")

# Link the executable with the DLL
target_link_libraries(matchmaking PRIVATE matchmaker)

# Copy DLL to the same directory as the executable
add_custom_command(
        TARGET matchmaker POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:matchmaker> $<TARGET_FILE_DIR:matchmaking>
        COMMENT "Copying matchmaker DLL to executable directory"
)