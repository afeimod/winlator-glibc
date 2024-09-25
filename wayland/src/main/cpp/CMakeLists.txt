cmake_minimum_required(VERSION 3.22.1)
project(wayland-server-android)
set(CMAKE_BUILD_TYPE Release)

set(SCRIPTS_BIN_DIR ${CMAKE_CURRENT_SOURCE_DIR}/scripts/bin)

include(scripts/utils.cmake)
include(scripts/wayland-scanner.cmake)

add_subdirectory(ffi)
add_subdirectory(pixman)
add_subdirectory(wayland)
add_subdirectory(wayland-protocols)
add_subdirectory(xkbcommon)
add_subdirectory(png)
add_subdirectory(cairo)
add_subdirectory(weston)
add_subdirectory(weston-wrapper)