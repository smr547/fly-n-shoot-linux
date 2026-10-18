cmake_minimum_required(VERSION 3.20)
project(fly_n_shoot_linux LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SDL3 CONFIG REQUIRED)

add_executable(fly_n_shoot_sdl_smoke smoke.cpp)
target_link_libraries(fly_n_shoot_sdl_smoke PRIVATE SDL3::SDL3)
target_compile_options(fly_n_shoot_sdl_smoke PRIVATE -Wall -Wextra -Wpedantic)



# QP/C++ 7.3.2 POSIX application. QP source is supplied by the user locally.
option(FLY_BUILD_QP_GAME "Build original QP/C++ game with SDL3 BSP" OFF)
set(QP_ROOT "$ENV{HOME}/qp_old/qpcpp" CACHE PATH "QP/C++ 7.3.2 source root")
if(FLY_BUILD_QP_GAME)
    if(NOT EXISTS "${QP_ROOT}/ports/posix/qf_port.cpp")
        message(FATAL_ERROR "QP/C++ POSIX port not found under QP_ROOT=${QP_ROOT}")
    endif()
    find_package(Threads REQUIRED)
    file(GLOB QP_QEP_SOURCES CONFIGURE_DEPENDS "${QP_ROOT}/src/qep/*.cpp")
    file(GLOB QP_QF_SOURCES CONFIGURE_DEPENDS "${QP_ROOT}/src/qf/*.cpp")
    add_library(qp_posix STATIC
        ${QP_QEP_SOURCES}
        ${QP_QF_SOURCES}
        "${QP_ROOT}/ports/posix/qf_port.cpp")
    target_include_directories(qp_posix PUBLIC
        "${QP_ROOT}/ports/posix"
        "${QP_ROOT}/include"
        "${QP_ROOT}/src")
    target_link_libraries(qp_posix PUBLIC Threads::Threads)

    set(GAME_SRC "${CMAKE_CURRENT_SOURCE_DIR}/../qwin-gui/game-gui")
    add_executable(fly_n_shoot_qp
        game_main.cpp bsp.cpp frame_bridge.cpp
        "${GAME_SRC}/ship.cpp"
        "${GAME_SRC}/missile.cpp"
        "${GAME_SRC}/mine1.cpp"
        "${GAME_SRC}/mine2.cpp"
        "${GAME_SRC}/tunnel.cpp")
    target_include_directories(fly_n_shoot_qp PRIVATE
        "${GAME_SRC}" "${CMAKE_CURRENT_SOURCE_DIR}")
    target_link_libraries(fly_n_shoot_qp PRIVATE qp_posix SDL3::SDL3)
    target_compile_options(fly_n_shoot_qp PRIVATE -Wall -Wextra -Wpedantic)
endif()
