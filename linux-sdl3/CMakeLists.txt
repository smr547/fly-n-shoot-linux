cmake_minimum_required(VERSION 3.20)
project(fly_n_shoot_linux LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SDL3 CONFIG REQUIRED)

add_executable(fly_n_shoot_sdl_smoke smoke.cpp)
target_link_libraries(fly_n_shoot_sdl_smoke PRIVATE SDL3::SDL3)
target_compile_options(fly_n_shoot_sdl_smoke PRIVATE -Wall -Wextra -Wpedantic)
