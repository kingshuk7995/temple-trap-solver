cmake_minimum_required(VERSION 3.10)

# ------------------------------------------------------------
# Project setup
# ------------------------------------------------------------
project(temple_trap LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Force static build
set(BUILD_SHARED_LIBS OFF)
set(SFML_STATIC_LIBRARIES TRUE)

# Default build type
if(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING
        "Choose the type of build: Debug, Release, RelWithDebInfo, MinSizeRel" FORCE)
endif()

# ------------------------------------------------------------
# Compiler flags
# ------------------------------------------------------------
if(MSVC)
    message(STATUS "Configuring for MSVC compiler (static runtime)")

    # Static runtime
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

    add_compile_definitions(_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS)
    add_compile_options(/W4 /EHsc /arch:AVX2)

    set(CMAKE_CXX_FLAGS_DEBUG "/Zi /Od /DDEBUG")
    set(CMAKE_CXX_FLAGS_RELEASE "/O2 /DNDEBUG /GL")

    set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /LTCG")
else()
    message(STATUS "Configuring for GCC/Clang")
    add_compile_options(-Wall -Wextra -Wpedantic -Werror=nonportable-include-path)
    set(CMAKE_CXX_FLAGS_DEBUG "-Og -g")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -march=native -DNDEBUG")
endif()

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

# ------------------------------------------------------------
# Fetch SFML (statically)
# ------------------------------------------------------------
include(FetchContent)
FetchContent_Declare(
  sfml
  GIT_REPOSITORY https://github.com/SFML/SFML.git
  GIT_TAG 2.6.1
)
FetchContent_MakeAvailable(sfml)

# Ensure SFML uses static runtime too (important!)
foreach(target sfml-system sfml-window sfml-graphics sfml-audio)
    if(TARGET ${target})
        set_target_properties(${target} PROPERTIES
            MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    endif()
endforeach()

# ------------------------------------------------------------
# Source files
# ------------------------------------------------------------
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    src/*.cpp
    src/*.hpp
)

add_executable(solver ${SOURCES})
target_include_directories(solver PRIVATE ${CMAKE_SOURCE_DIR}/src)

# ------------------------------------------------------------
# Link SFML and dependencies
# ------------------------------------------------------------
target_link_libraries(solver PRIVATE sfml-graphics)

if(MSVC)
    target_link_libraries(solver PRIVATE
        opengl32
        winmm
        gdi32
        imm32
        ole32
        ws2_32
        shell32
    )
endif()

# ------------------------------------------------------------
# Output directories
# ------------------------------------------------------------
set_target_properties(solver PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY_DEBUG   ${CMAKE_BINARY_DIR}/bin/Debug
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin/Release
)
