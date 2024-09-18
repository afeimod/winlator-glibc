cmake_minimum_required(VERSION 3.22.1)
project(weston-wrapper)

# weston-wrapper-shared
add_library(
        weston-wrapper-shared STATIC
        weston-wrapper.c
)

target_link_libraries(
        weston-wrapper-shared PUBLIC
        weston-public
        pixman
        android
        log
)

target_include_directories(
        weston-wrapper-shared PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

# weston-jni
add_library(
        weston-jni SHARED
        weston-jni.c
)

target_link_libraries(
        weston-jni PUBLIC
        weston-wrapper-shared
        weston
        pixman
        android
        log
)

target_include_directories(
        weston-jni PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)