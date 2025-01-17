cmake_minimum_required(VERSION 3.29)
project(Atlas)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(ATLAS_WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
add_definitions(-DATLAS_WORKING_DIRECTORY="${ATLAS_WORKING_DIRECTORY}")

add_definitions(-D_WIN32_WINNT=0x0A00)

if(MSVC)
    add_compile_options(/bigobj)
endif()

# Add subdirectories
add_subdirectory(engine)
add_subdirectory(matchmaker)
add_subdirectory(client)
add_subdirectory(server)

