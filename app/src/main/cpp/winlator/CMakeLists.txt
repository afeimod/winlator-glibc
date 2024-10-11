cmake_minimum_required(VERSION 3.22.1)

project(Winlator)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2 -Wno-unused-function -Wimplicit-function-declaration")

add_library(
        winlator SHARED
        xr/engine.c
        xr/framebuffer.c
        xr/input.c
        xr/main.c
        xr/math.c
        xr/renderer.c
        drawable.c
        gpu_image.c
        sysvshared_memory.c
        xconnector_epoll.c
        alsa_client.c
        patchelf_wrapper.cpp
)

target_link_libraries(
        winlator PUBLIC
        log
        android
        jnigraphics
        openxr_loader
        aaudio
        EGL
        GLESv2
        GLESv3
)