cmake_minimum_required(VERSION 3.10)

# Project name
project(temple_trap LANGUAGES CXX)

# C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# For single-config generators (Make, Ninja)
if(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING
        "Choose the type of build: Debug, Release, RelWithDebInfo, MinSizeRel" FORCE)
endif()

# Set compiler flags per configuration
set(CMAKE_CXX_FLAGS_DEBUG "-Wall -Wextra -Og -g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -march=native -DNDEBUG -g")

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    src/*.cpp
    src/*.hpp
)

add_executable(solver ${SOURCES})

set_target_properties(solver PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY_DEBUG   ${CMAKE_BINARY_DIR}/bin/Debug
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin/Release
)
