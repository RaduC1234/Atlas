cmake_minimum_required(VERSION 3.28)
project(Client)

file(GLOB_RECURSE SERVER_SOURCES "src/*.cpp" "src/*.hpp")

add_executable(client ${SERVER_SOURCES})

target_link_libraries(client PRIVATE engine)

target_include_directories(client PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

target_compile_features(client PRIVATE cxx_std_20)

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
