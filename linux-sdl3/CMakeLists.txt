cmake_minimum_required(VERSION 3.20)
project(fly_n_shoot_linux LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SDL3 CONFIG REQUIRED)

add_executable(fly_n_shoot_sdl_smoke smoke.cpp)
target_link_libraries(fly_n_shoot_sdl_smoke PRIVATE SDL3::SDL3)
target_compile_options(fly_n_shoot_sdl_smoke PRIVATE -Wall -Wextra -Wpedantic)


# The original QM-generated game is deliberately kept separate from the
# SDL smoke test. The QP build is opt-in until the POSIX BSP is complete.
option(FLY_BUILD_QP_GAME "Build original QP/C++ game with SDL3 BSP" OFF)
if(FLY_BUILD_QP_GAME)
    if(NOT DEFINED QP_ROOT)
        message(FATAL_ERROR "Set -DQP_ROOT=/path/to/qpcpp (QP/C++ 7.3.2)")
    endif()
    set(QP_PORT "posix" CACHE STRING "QP/C++ POSIX port directory")
    set(QP_LIB "${QP_ROOT}/ports/${QP_PORT}" CACHE PATH
        "QP POSIX port location (verify against local QP tree)")
    message(STATUS "QP/C++ source: ${QP_ROOT}")
    message(STATUS "QP/C++ port: ${QP_LIB}")
    message(FATAL_ERROR
        "QP game target not yet enabled: POSIX port sources, lifecycle "
        "and SDL framebuffer bridge must be verified together. "
        "The SDL smoke target remains buildable.")
endif()
