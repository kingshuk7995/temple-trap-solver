cmake_minimum_required(VERSION 3.10)

project(temple_trap LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(BUILD_SHARED_LIBS OFF)
set(SFML_STATIC_LIBRARIES TRUE)

set(IS_EMSCRIPTEN FALSE)
if("${CMAKE_SYSTEM_NAME}" STREQUAL "Emscripten" OR EMSCRIPTEN)
    set(IS_EMSCRIPTEN TRUE)
    message(STATUS "Configuring for Emscripten (WASM) build")
else()
    message(STATUS "Configuring for native build")
endif()

if(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING
        "Debug, Release, RelWithDebInfo, MinSizeRel" FORCE)
endif()

function(enable_strict_warnings target)
    if(MSVC)
        target_compile_options(${target} PRIVATE /W4 /permissive- /EHsc)
    else()
        target_compile_options(${target} PRIVATE
            -Wall -Wextra -Wpedantic
            -Wshadow
            -Wconversion
            -Wsign-conversion
            -Wdouble-promotion
        )
    endif()
endfunction()

if(MSVC)
    message(STATUS "MSVC compilation detected")

    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    set(CMAKE_CXX_FLAGS_DEBUG "/Zi /Od /DDEBUG")
    set(CMAKE_CXX_FLAGS_RELEASE "/O2 /DNDEBUG /GL")
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /LTCG")

else()
    message(STATUS "GCC/Clang detected")

    if(NOT IS_EMSCRIPTEN)
        set(CMAKE_CXX_FLAGS_DEBUG "-Og -g")
        set(CMAKE_CXX_FLAGS_RELEASE "-O3 -march=native -DNDEBUG")
    else()
        set(CMAKE_CXX_FLAGS_DEBUG "-g")
        set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
    endif()
endif()

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

if(NOT IS_EMSCRIPTEN)
    include(FetchContent)
    FetchContent_Declare(
        sfml
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG 2.6.1
    )
    FetchContent_MakeAvailable(sfml)

    # Disable warnings inside SFML
    foreach(lib sfml-system sfml-window sfml-graphics sfml-audio)
        if(TARGET ${lib})
            if(MSVC)
                target_compile_options(${lib} PRIVATE /w)
            else()
                target_compile_options(${lib} PRIVATE -w)
            endif()

            if(MSVC)
                set_target_properties(${lib} PROPERTIES
                    MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
            endif()
        endif()
    endforeach()
endif()

set(SRC_CORE
    src/core/board.hpp
    src/core/solver.hpp
    src/core/types.hpp
)

if(NOT IS_EMSCRIPTEN)
    set(SRC_CLI
        src/cli/input.hpp
        src/cli/renderer.hpp
        src/cli/main.cpp
    )

    add_executable(solver ${SRC_CORE} ${SRC_CLI})

    target_include_directories(solver PRIVATE
        src/core
        src/cli
    )

    target_link_libraries(solver PRIVATE sfml-graphics)
    enable_strict_warnings(solver)

    if(MSVC)
        target_link_libraries(solver PRIVATE
            opengl32 winmm gdi32 imm32 ole32 ws2_32 shell32
        )
    endif()

    set_target_properties(solver PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY_DEBUG   ${CMAKE_BINARY_DIR}/bin/Debug
        RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin/Release
    )

endif()

if(IS_EMSCRIPTEN)
    add_executable(web_app
        src/wasm/bindings.cpp
    )

    target_include_directories(web_app PRIVATE src/core)
    target_compile_definitions(web_app PRIVATE USING_EMSCRIPTEN)

    target_link_options(web_app PRIVATE
        "--bind"
        "-sMODULARIZE=1"
        "-sEXPORT_NAME=\"WebAppModule\""
        "-sALLOW_MEMORY_GROWTH=1"
    )

    set_target_properties(web_app PROPERTIES
        OUTPUT_NAME "temple_trap_solver"
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/wasm
    )

    enable_strict_warnings(web_app)

    set(VITE_WASM_DIR "${CMAKE_SOURCE_DIR}/temple-trap-ui/public")

    add_custom_command(
        TARGET web_app POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "${VITE_WASM_DIR}"

        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "$<TARGET_FILE:web_app>"
            "${VITE_WASM_DIR}/$<TARGET_FILE_NAME:web_app>"

        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "$<TARGET_FILE_DIR:web_app>/$<TARGET_FILE_BASE_NAME:web_app>.wasm"
            "${VITE_WASM_DIR}/$<TARGET_FILE_BASE_NAME:web_app>.wasm"

        COMMENT "Copying Emscripten output to React public/ directory"
    )

    message(STATUS "WASM build target configured ✔")
endif()
