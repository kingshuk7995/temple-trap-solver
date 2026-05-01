cmake_minimum_required(VERSION 3.15)

# ------------------------------------------------------------
# Project setup
# ------------------------------------------------------------
project(temple_trap LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Default build type
if(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING
        "Choose the type of build: Debug, Release, RelWithDebInfo, MinSizeRel" FORCE)
endif()

# ------------------------------------------------------------
# Compiler flags
# ------------------------------------------------------------
if(MSVC)
    message(STATUS "Configuring for MSVC compiler")
    add_compile_definitions(_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS)
    add_compile_options(/W4 /EHsc /arch:AVX2)
else()
    message(STATUS "Configuring for GCC/Clang")
    add_compile_options(-Wall -Wextra -Wpedantic)
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -march=native -DNDEBUG")
endif()

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

# ------------------------------------------------------------
# Fetch nanobind
# ------------------------------------------------------------
find_package(Python COMPONENTS Interpreter Development.Module REQUIRED)

include(FetchContent)
FetchContent_Declare(
  nanobind
  GIT_REPOSITORY https://github.com/wjakob/nanobind.git
  GIT_TAG v2.0.0
)
FetchContent_MakeAvailable(nanobind)

# ------------------------------------------------------------
# Source files
# ------------------------------------------------------------
set(SOURCES
    src/bindings.cpp
)

nanobind_add_module(temple_trap_core ${SOURCES})
target_include_directories(temple_trap_core PRIVATE ${CMAKE_SOURCE_DIR}/src)

