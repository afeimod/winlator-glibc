cmake_minimum_required(VERSION 3.22.1)
project(wayland)

set(WAYLAND_PACKAGE "wayland")
set(WAYLAND_VERSION "1.23.1")

set(WAYLAND_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(WAYLAND_SRC_DIR ${WAYLAND_DIR}/repo/src)
set(WAYLAND_PROTOCOL_DIR ${WAYLAND_DIR}/repo/protocol)
set(AUTOGEN_DIR ${WAYLAND_DIR}/autogen)
set(TMP_DIR ${AUTOGEN_DIR}/tmp)
set(PROTOCOL_INSTALL_DIR ${WAYLAND_DIR}/protocol)

set(CONFIG_H_IN ${WAYLAND_DIR}/config.h.in)
set(CONFIG_H_DEST ${AUTOGEN_DIR}/config.h)

set(VERSION_H_IN ${WAYLAND_SRC_DIR}/wayland-version.h.in)
set(VERSION_H_DEST ${AUTOGEN_DIR}/wayland-version.h)

set(WAYLAND_OS_C ${WAYLAND_SRC_DIR}/wayland-os.c)
set(WAYLAND_OS_C_TMP ${TMP_DIR}/wayland-os.c)

include(CheckFunctionExists)
include(CheckIncludeFile)
include(CheckStructHasMember)

# Check if need to update submodule
check_repo(${WAYLAND_DIR}/repo)

# Install protocols via wayland-scanner
set(PROTOCOL_WAYLAND_XML ${WAYLAND_PROTOCOL_DIR}/wayland.xml)
file(MAKE_DIRECTORY ${PROTOCOL_INSTALL_DIR})
wayland_scanner("" "server-header" "${PROTOCOL_WAYLAND_XML}"
        "${PROTOCOL_INSTALL_DIR}/wayland-server-protocol.h")
wayland_scanner("-c" "server-header" "${PROTOCOL_WAYLAND_XML}"
        "${PROTOCOL_INSTALL_DIR}/wayland-server-protocol-core.h")
wayland_scanner("" "client-header" "${PROTOCOL_WAYLAND_XML}"
        "${PROTOCOL_INSTALL_DIR}/wayland-client-protocol.h")
wayland_scanner("-c" "client-header" "${PROTOCOL_WAYLAND_XML}"
        "${PROTOCOL_INSTALL_DIR}/wayland-client-protocol-core.h")
wayland_scanner("-s" "public-code" "${PROTOCOL_WAYLAND_XML}"
        "${PROTOCOL_INSTALL_DIR}/wayland-protocol.c")

# Generate config.h
file(MAKE_DIRECTORY ${AUTOGEN_DIR})
check_function_exists(accept4 HAVE_ACCEPT4)
check_function_exists(memfd_create HAVE_MEMFD_CREATE)
check_function_exists(mkostemp HAVE_MKOSTEMP)
check_function_exists(mremap HAVE_MREMAP)
check_function_exists(posix_fallocate HAVE_POSIX_FALLOCATE)
check_function_exists(prctl HAVE_PRCTL)
check_function_exists(strndup HAVE_STRNDUP)
check_include_file("sys/prctl.h" HAVE_SYS_PRCTL_H)
check_include_file("sys/procctl.h" HAVE_PROCCTL_H)
check_include_file("sys/ucred.h" HAVE_SYS_UCRED_H)
check_struct_has_member("struct xucred" "cr_pid" "sys/ucred.h" XUCRED_CR_PID_PRESENT LANGUAGE C)
if (DEFINED XUCRED_CR_PID_PRESENT)
    set(HAVE_XUCRED_CR_PID 1)
else ()
    set(HAVE_XUCRED_CR_PID 0)
endif ()
set(HAVE_BROKEN_MSG_CMSG_CLOEXEC "0")
set(PACKAGE ${WAYLAND_PACKAGE})
set(PACKAGE_VERSION ${WAYLAND_VERSION})
configure_file(
        ${CONFIG_H_IN}
        ${CONFIG_H_DEST}
        @ONLY
)

# Generate wayland-version.h
string(REPLACE "." ";" VERSION_LIST "${WAYLAND_VERSION}")
list(GET VERSION_LIST 0 WAYLAND_VERSION_MAJOR)
list(GET VERSION_LIST 1 WAYLAND_VERSION_MINOR)
list(GET VERSION_LIST 2 WAYLAND_VERSION_MICRO)
configure_file(
        ${VERSION_H_IN}
        ${VERSION_H_DEST}
        @ONLY
)

# C args
add_compile_options(
        -O2
        -std=c99
        -D_POSIX_C_SOURCE=200809L
        -Wno-unused-parameter
        -Wstrict-prototypes
        -Wmissing-prototypes
        -fvisibility=hidden
)

# wayland-util
add_library(
        wayland-util STATIC
        ${WAYLAND_SRC_DIR}/wayland-util.c
)

target_include_directories(wayland-util PRIVATE ${WAYLAND_SRC_DIR})

# wayland-private
configure_file(
        ${WAYLAND_OS_C}
        ${WAYLAND_OS_C_TMP}
        COPYONLY
)

add_library(
        wayland-private STATIC
        ${WAYLAND_SRC_DIR}/connection.c
        ${WAYLAND_OS_C_TMP}
)

target_link_libraries(
        wayland-private PRIVATE
        ffi
)

target_include_directories(wayland-private PRIVATE ${WAYLAND_SRC_DIR})

# wayland-server
add_library(
        wayland-server SHARED
        ${WAYLAND_SRC_DIR}/wayland-server.c
        ${WAYLAND_SRC_DIR}/wayland-shm.c
        ${WAYLAND_SRC_DIR}/event-loop.c
        ${PROTOCOL_INSTALL_DIR}/wayland-protocol.c
)

target_link_libraries(
        wayland-server PRIVATE
        ffi
        wayland-util
        wayland-private
)

target_include_directories(
        wayland-server PUBLIC
        ${WAYLAND_SRC_DIR}
        ${PROTOCOL_INSTALL_DIR}
)

target_include_directories(wayland-server PRIVATE ${AUTOGEN_DIR})

# wayland-client
add_library(
        wayland-client SHARED
        ${WAYLAND_SRC_DIR}/wayland-client.c
        ${PROTOCOL_INSTALL_DIR}/wayland-protocol.c
)

target_link_libraries(wayland-client PRIVATE
        ffi
        wayland-util
        wayland-private
)

target_include_directories(wayland-client PUBLIC
        ${WAYLAND_SRC_DIR}
        ${PROTOCOL_INSTALL_DIR}
)

target_include_directories(wayland-client PRIVATE ${AUTOGEN_DIR})