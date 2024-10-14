cmake_minimum_required(VERSION 3.22.1)
project(weston)

set(WESTON_PACKAGE "weston")
set(WESTON_VERSION "14.0.0")

set(WESTON_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(WESTON_SRC_DIR ${WESTON_DIR}/repo/libweston)
set(WESTON_SHARED_DIR ${WESTON_DIR}/repo/shared)
set(WESTON_PROTOCOL_DIR ${WESTON_DIR}/repo/protocol)
set(WESTON_INCLUDE_DIR ${WESTON_DIR}/repo/include)
set(AUTOGEN_DIR ${WESTON_DIR}/autogen)
set(PROTOCOL_INSTALL_DIR ${WESTON_DIR}/protocol)

set(CONFIG_H_IN ${WESTON_DIR}/config.h.in)
set(CONFIG_H_DEST ${AUTOGEN_DIR}/config/config.h)

set(GIT_VERSION_H_IN ${WESTON_SRC_DIR}/git-version.h.meson)
set(GIT_VERSION_H_DEST ${AUTOGEN_DIR}/config/git-version.h)

set(VERSION_H_IN ${WESTON_INCLUDE_DIR}/libweston/version.h.in)
set(VERSION_H_DEST ${AUTOGEN_DIR}/include/libweston/version.h)

include(CheckFunctionExists)
include(CheckIncludeFile)

# Check if need to update submodule
check_repo(${WESTON_DIR}/repo)

# Check
set(MAJOR_IN_SYSMACROS 1)
set(MAJOR_IN_MKDEV 1)
set(_GNU_SOURCE 1)
set(_ALL_SOURCE 1)
set(EGL_NO_X11 1)
set(MESA_EGL_NO_X11_HEADERS 1)
set(EGL_NO_PLATFORM_SPECIFIC_TYPES 1)
set(PACKAGE_STRING ON)
set(PACKAGE_VERSION ON)
set(VERSION ON)
set(PACKAGE_URL "https://wayland.freedesktop.org")
set(PACKAGE_BUGREPORT "https://gitlab.freedesktop.org/wayland/weston/issues/")
set(BINDIR " ")
set(DATADIR " ")
set(LIBEXECDIR " ")
set(MODULEDIR " ")
set(LIBWESTON_MODULEDIR " ")
set(WESTON_NATIVE_BACKEND " ")
set(HAVE_PANGO 0)
set(HAVE_JPEG 0)
set(HAVE_WEBP 0)
set(HAVE_XWAYLAND_LISTENFD 0)
set(BUILD_XWAYLAND 0)
set(XSERVER_PATH 0)
set(WESTON_SHELL_CLIENT " ")
check_function_exists(mkostemp HAVE_MKOSTEMP)
check_function_exists(strchrnul HAVE_STRCHRNUL)
check_function_exists(initgroups HAVE_INITGROUPS)
check_function_exists(posix_fallocate HAVE_POSIX_FALLOCATE)
check_function_exists(memfd_create HAVE_MEMFD_CREATE)
check_function_exists(unreachable HAVE_UNREACHABLE)
check_include_file("linux/sync_file.h" HAVE_LINUX_SYNC_FILE_H)
check_c_source_compiles("
    int main(int argc, char** argv) {return __builtin_clz(1);}
" HAVE_BUILTIN_CLZ)
check_c_source_compiles("
    int main(int argc, char** argv) {return __builtin_bswap32(0);}
" HAVE_BUILTIN_BSWAP32)
check_c_source_compiles("
    int main(int argc, char** argv) {return __builtin_popcount(0);}
" HAVE_BUILTIN_POPCOUNT)

set(VCS_TAG ${WESTON_VERSION})

file(MAKE_DIRECTORY ${PROTOCOL_INSTALL_DIR})
file(MAKE_DIRECTORY ${AUTOGEN_DIR})
file(MAKE_DIRECTORY ${AUTOGEN_DIR}/include)
file(MAKE_DIRECTORY ${AUTOGEN_DIR}/include/libweston)
file(MAKE_DIRECTORY ${AUTOGEN_DIR}/config)

# Generate config.h
configure_file(
        ${CONFIG_H_IN}
        ${CONFIG_H_DEST}
        @ONLY
)

# Generate git-version.h
configure_file(
        ${GIT_VERSION_H_IN}
        ${GIT_VERSION_H_DEST}
        @ONLY
)

# Generate version.h
string(REPLACE "." ";" VERSION_LIST "${WESTON_VERSION}")
list(GET VERSION_LIST 0 WESTON_VERSION_MAJOR)
list(GET VERSION_LIST 1 WESTON_VERSION_MINOR)
list(GET VERSION_LIST 2 WESTON_VERSION_MICRO)
configure_file(
        ${VERSION_H_IN}
        ${VERSION_H_DEST}
        @ONLY
)

# Install weston protocols
file(GLOB_RECURSE WESTON_INTERNAL_PROTOCOLS "${WESTON_PROTOCOL_DIR}/*.xml")
foreach (PROTOCOL_XML IN LISTS WESTON_INTERNAL_PROTOCOLS)
    get_filename_component(BASE_NAME ${PROTOCOL_XML} NAME_WE)

    # generate protocol files by wayland-scanner
    wayland_scanner("" "client-header" ${PROTOCOL_XML} "${PROTOCOL_INSTALL_DIR}/${BASE_NAME}-client-protocol.h")
    wayland_scanner("" "server-header" ${PROTOCOL_XML} "${PROTOCOL_INSTALL_DIR}/${BASE_NAME}-server-protocol.h")
    wayland_scanner("" "public-code" ${PROTOCOL_XML} "${PROTOCOL_INSTALL_DIR}/${BASE_NAME}-protocol.c")

    set(TARGET_NAME "weston-${BASE_NAME}")

    add_library(${TARGET_NAME} STATIC ${PROTOCOL_INSTALL_DIR}/${BASE_NAME}-protocol.c)
    target_link_libraries(${TARGET_NAME} PRIVATE wayland-util wayland-protocol-core)
    target_include_directories(${TARGET_NAME} PUBLIC ${PROTOCOL_INSTALL_DIR})
endforeach ()

# Install headers
set(WESTON_HEADERS
        ${WESTON_INCLUDE_DIR}/libweston/config-parser.h
        ${WESTON_INCLUDE_DIR}/libweston/libweston.h
        ${WESTON_INCLUDE_DIR}/libweston/desktop.h
        ${WESTON_INCLUDE_DIR}/libweston/matrix.h
        ${WESTON_INCLUDE_DIR}/libweston/plugin-registry.h
        ${WESTON_INCLUDE_DIR}/libweston/windowed-output-api.h
        ${WESTON_INCLUDE_DIR}/libweston/weston-log.h
        ${WESTON_INCLUDE_DIR}/libweston/zalloc.h
        ${WESTON_INCLUDE_DIR}/libweston/remoting-plugin.h
        ${WESTON_INCLUDE_DIR}/libweston/pipewire-plugin.h
        ${WESTON_INCLUDE_DIR}/libweston/shell-utils.h
        ${WESTON_INCLUDE_DIR}/libweston/backend-android.h
)
foreach (HEADER_FILE IN LISTS WESTON_HEADERS)
    configure_file(
            ${HEADER_FILE}
            ${AUTOGEN_DIR}/include/libweston
            COPYONLY
    )
endforeach ()

# C args
add_compile_options(
        -Wmissing-prototypes
        -Wno-unused-parameter
        -Wno-shift-negative-value # required due to Pixman
        -Wno-missing-field-initializers
        -Wno-pedantic
        -Wundef
        -fvisibility=hidden
)

# libweston-shared
add_library(
        weston-shared STATIC
        ${WESTON_SHARED_DIR}/config-parser.c
        ${WESTON_SHARED_DIR}/option-parser.c
        ${WESTON_SHARED_DIR}/file-util.c
        ${WESTON_SHARED_DIR}/os-compatibility.c
        ${WESTON_SHARED_DIR}/process-util.c
        ${WESTON_SHARED_DIR}/hash.c
)

target_link_libraries(
        weston-shared PUBLIC
        wayland-client
        pixman
        xkbcommon
        egl
)

target_include_directories(
        weston-shared PUBLIC
        ${AUTOGEN_DIR}/include
)

target_include_directories(
        weston-shared PRIVATE
        ${AUTOGEN_DIR}/config
        ${WESTON_SHARED_DIR}
)

# libweston
add_library(
        weston SHARED
        # libweston
        ${WESTON_SRC_DIR}/animation.c
        ${WESTON_SRC_DIR}/auth.c
        ${WESTON_SRC_DIR}/bindings.c
        ${WESTON_SRC_DIR}/clipboard.c
        ${WESTON_SRC_DIR}/color.c
        ${WESTON_SRC_DIR}/color-properties.c
        ${WESTON_SRC_DIR}/color-management.c
        ${WESTON_SRC_DIR}/color-noop.c
        ${WESTON_SRC_DIR}/color-profile-param-builder.c
        ${WESTON_SRC_DIR}/compositor.c
        ${WESTON_SRC_DIR}/content-protection.c
        ${WESTON_SRC_DIR}/data-device.c
        ${WESTON_SRC_DIR}/drm-formats.c
        ${WESTON_SRC_DIR}/id-number-allocator.c
        ${WESTON_SRC_DIR}/input.c
        ${WESTON_SRC_DIR}/linux-dmabuf.c
        ${WESTON_SRC_DIR}/linux-explicit-synchronization.c
        ${WESTON_SRC_DIR}/linux-sync-file.c
        ${WESTON_SRC_DIR}/log.c
        ${WESTON_SRC_DIR}/noop-renderer.c
        ${WESTON_SRC_DIR}/output-capture.c
        ${WESTON_SRC_DIR}/pixel-formats.c
        ${WESTON_SRC_DIR}/pixman-renderer.c
        ${WESTON_SRC_DIR}/plugin-registry.c
        ${WESTON_SRC_DIR}/screenshooter.c
        ${WESTON_SRC_DIR}/timeline.c
        ${WESTON_SRC_DIR}/touch-calibration.c
        ${WESTON_SRC_DIR}/weston-log-wayland.c
        ${WESTON_SRC_DIR}/weston-log-file.c
        ${WESTON_SRC_DIR}/weston-log-flight-rec.c
        ${WESTON_SRC_DIR}/weston-log.c
        ${WESTON_SRC_DIR}/weston-direct-display.c
        ${WESTON_SHARED_DIR}/matrix.c
        # libweston/desktop
        ${WESTON_SRC_DIR}/desktop/libweston-desktop.c
        ${WESTON_SRC_DIR}/desktop/client.c
        ${WESTON_SRC_DIR}/desktop/seat.c
        ${WESTON_SRC_DIR}/desktop/surface.c
        ${WESTON_SRC_DIR}/desktop/xwayland.c
        ${WESTON_SRC_DIR}/desktop/xdg-shell.c
        ${WESTON_SRC_DIR}/desktop/xdg-shell-v6.c
        # libweston/shell-utils
        ${WESTON_SRC_DIR}/shell-utils/shell-utils.c
)

target_link_libraries(
        weston PUBLIC
        wayland-server
        pixman
        xkbcommon
        egl
        weston-shared
        # libweston
        weston-color-management-v1
        wayland-protocols-linux-dmabuf-unstable-v1
        wayland-protocols-linux-explicit-synchronization-unstable-v1
        wayland-protocols-input-method-unstable-v1
        wayland-protocols-input-timestamps-unstable-v1
        wayland-protocols-presentation-time
        wayland-protocols-pointer-constraints-unstable-v1
        wayland-protocols-relative-pointer-unstable-v1
        wayland-protocols-single-pixel-buffer-v1
        wayland-protocols-tearing-control-v1
        weston-text-cursor-position
        wayland-protocols-text-input-unstable-v1
        weston-weston-touch-calibration
        weston-weston-content-protection
        wayland-protocols-viewporter
        wayland-protocols-xdg-output-unstable-v1
        weston-weston-debug
        weston-weston-direct-display
        weston-weston-output-capture
        wayland-protocols-tablet-unstable-v2
        # libweston/desktop
        wayland-protocols-xdg-shell-unstable-v6
        wayland-protocols-xdg-shell
)

target_include_directories(
        weston PUBLIC
        ${AUTOGEN_DIR}/include
)

target_include_directories(
        weston PRIVATE
        ${AUTOGEN_DIR}/config
        ${WESTON_SRC_DIR}
        ${WESTON_DIR}/repo
)

# libcairo-shared
add_library(
        cairo-shared STATIC
        ${WESTON_SHARED_DIR}/image-loader.c
        ${WESTON_SHARED_DIR}/cairo-util.c
        ${WESTON_SHARED_DIR}/frame.c
)

target_link_libraries(
        cairo-shared PUBLIC
        weston-shared
        cairo
        png
        pixman
)

target_include_directories(
        cairo-shared PRIVATE
        ${AUTOGEN_DIR}/config
        ${WESTON_DIR}/repo
)

# libgl-borders
add_library(
        gl-borders STATIC
        ${WESTON_SRC_DIR}/gl-borders.c
)

target_link_libraries(
        gl-borders PUBLIC
        cairo-shared
        egl
)

target_include_directories(
        gl-borders PUBLIC
        ${AUTOGEN_DIR}/include
)

target_include_directories(
        gl-borders PRIVATE
        ${WESTON_SRC_DIR}
        ${WESTON_DIR}/repo
        ${AUTOGEN_DIR}/config
)

# libbackend-android
add_library(
        backend-android SHARED
        ${WESTON_SRC_DIR}/backend-android/android.c
)

target_link_libraries(
        backend-android PUBLIC
        weston
        gl-borders
        # weston-wrapper
        weston-wrapper-shared
)

target_include_directories(
        backend-android PRIVATE
        ${WESTON_SRC_DIR}
        ${WESTON_DIR}/repo
        ${AUTOGEN_DIR}/config
)

set_target_properties(backend-android PROPERTIES PREFIX "")

# libfrontend-android
add_library(
        frontend-android STATIC
        ${WESTON_DIR}/repo/frontend/android.c
)

target_link_libraries(
        frontend-android PUBLIC
        weston
)

target_include_directories(
        frontend-android PRIVATE
        ${AUTOGEN_DIR}/config
)

# libfullscreen-shell
add_library(
        fullscreen-shell SHARED
        ${WESTON_DIR}/repo/fullscreen-shell/fullscreen-shell.c
)

target_link_libraries(
        fullscreen-shell PUBLIC
        weston
        frontend-android
        wayland-protocols-fullscreen-shell-unstable-v1
)

target_include_directories(
        fullscreen-shell PUBLIC
        ${AUTOGEN_DIR}/include
)

target_include_directories(
        fullscreen-shell PRIVATE
        ${WESTON_SRC_DIR}
        ${WESTON_DIR}/repo
        ${AUTOGEN_DIR}/config
)

set_target_properties(fullscreen-shell PROPERTIES PREFIX "")

# libdesktop-shell
add_library(
        desktop-shell SHARED
        ${WESTON_DIR}/repo/desktop-shell/shell.c
        ${WESTON_DIR}/repo/desktop-shell/input-panel.c
)

target_link_libraries(
        desktop-shell PUBLIC
        weston
        frontend-android
        weston-weston-desktop-shell
        wayland-protocols-input-method-unstable-v1
)

target_include_directories(
        desktop-shell PUBLIC
        ${AUTOGEN_DIR}/include
)

target_include_directories(
        desktop-shell PRIVATE
        ${WESTON_SRC_DIR}
        ${WESTON_INCLUDE_DIR}
        ${WESTON_DIR}/repo
        ${AUTOGEN_DIR}/config
)

set_target_properties(desktop-shell PROPERTIES PREFIX "")