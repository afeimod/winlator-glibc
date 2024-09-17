cmake_minimum_required(VERSION 3.22.1)
project(weston-wrapper)

# weston-wrapper-shared
add_library(
        weston-wrapper-shared STATIC
        weston-wrapper.cpp
)

target_link_libraries(
        weston-wrapper-shared PUBLIC
        pixman
        android
        log
)

target_include_directories(
        weston-wrapper-shared PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)