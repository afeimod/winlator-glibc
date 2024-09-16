cmake_minimum_required(VERSION 3.22.1)
project(wayland-protocols)

set(WAYLAND_PROTOCOLS_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(PROTOCOLS_DIR ${WAYLAND_PROTOCOLS_DIR}/repo)
set(PROTOCOLS_INSTALL_DIR ${WAYLAND_PROTOCOLS_DIR}/protocol)

# Check if need to update submodule
check_repo(${WAYLAND_PROTOCOLS_DIR}/repo)

# list all protocols here. "[type1],[name1],[version1];[type2],[name2],[version2]"
set(PROTOCOLS_LIST_STR "
        stable,     linux-dmabuf,                   v1  ;
        stable,     presentation-time,                  ;
        stable,     tablet,                         v2  ;
        stable,     viewporter,                         ;
        stable,     xdg-shell,                          ;
        staging,    alpha-modifier,                 v1  ;
        staging,    content-type,                   v1  ;
        staging,    cursor-shape,                   v1  ;
        staging,    drm-lease,                      v1  ;
        staging,    ext-foreign-toplevel-list,      v1  ;
        staging,    ext-idle-notify,                v1  ;
        staging,    ext-image-capture-source,       v1  ;
        staging,    ext-image-copy-capture,         v1  ;
        staging,    ext-session-lock,               v1  ;
        staging,    ext-transient-seat,             v1  ;
        staging,    fractional-scale,               v1  ;
        staging,    linux-drm-syncobj,              v1  ;
        staging,    security-context,               v1  ;
        staging,    single-pixel-buffer,            v1  ;
        staging,    tearing-control,                v1  ;
        staging,    xdg-activation,                 v1  ;
        staging,    xdg-dialog,                     v1  ;
        staging,    xdg-toplevel-drag,              v1  ;
        staging,    xdg-toplevel-icon,              v1  ;
        staging,    xwayland-shell,                 v1  ;
        unstable,   fullscreen-shell,               v1  ;
        unstable,   idle-inhibit,                   v1  ;
        unstable,   input-method,                   v1  ;
        unstable,   input-timestamps,               v1  ;
        unstable,   keyboard-shortcuts-inhibit,     v1  ;
        unstable,   linux-dmabuf,                   v1  ;
        unstable,   linux-explicit-synchronization, v1  ;
        unstable,   pointer-constraints,            v1  ;
        unstable,   pointer-gestures,               v1  ;
        unstable,   primary-selection,              v1  ;
        unstable,   relative-pointer,               v1  ;
        unstable,   tablet,                         v1  ;
        unstable,   tablet,                         v2  ;
        unstable,   text-input,                     v1  ;
        unstable,   text-input,                     v3  ;
        unstable,   xdg-decoration,                 v1  ;
        unstable,   xdg-foreign,                    v1  ;
        unstable,   xdg-foreign,                    v2  ;
        unstable,   xdg-output,                     v1  ;
        unstable,   xdg-shell,                      v5  ;
        unstable,   xdg-shell,                      v6  ;
        unstable,   xwayland-keyboard-grab,         v1
")

# Install all protocols
string(REPLACE " " "" PROTOCOLS_LIST_FLAT "${PROTOCOLS_LIST_STR}")
string(REPLACE "\n" "" PROTOCOLS_LIST_FLAT "${PROTOCOLS_LIST_FLAT}")
foreach (PROTOCOL_ITEM_STR IN LISTS PROTOCOLS_LIST_FLAT)
    string(REPLACE "," ";" PROTOCOL_ITEM_FLAT "${PROTOCOL_ITEM_STR}")
    list(GET PROTOCOL_ITEM_FLAT 0 PROTOCOL_TYPE)
    list(GET PROTOCOL_ITEM_FLAT 1 PROTOCOL_NAME)
    list(GET PROTOCOL_ITEM_FLAT 2 PROTOCOL_VERSION)

    # For stable type, there is no version code in file name.
    # For staging type, there iss a version code in file name.
    # For unstable type, there are "-unstable-" and version code in file name.
    set(PROTOCOL_XML_NAME ${PROTOCOL_NAME})
    if (PROTOCOL_TYPE STREQUAL "unstable")
        string(APPEND PROTOCOL_XML_NAME "-unstable")
    endif ()
    if (NOT PROTOCOL_VERSION STREQUAL "")
        string(APPEND PROTOCOL_XML_NAME "-${PROTOCOL_VERSION}")
    endif ()
    string(APPEND PROTOCOL_XML_NAME ".xml")

    set(PROTOCOL_XML_PATH "${PROTOCOLS_DIR}/${PROTOCOL_TYPE}/${PROTOCOL_NAME}/${PROTOCOL_XML_NAME}")
    set(PROTOCOL_INSTALL_DIR "${PROTOCOLS_INSTALL_DIR}/${PROTOCOL_TYPE}/${PROTOCOL_NAME}")

    if (NOT PROTOCOL_TYPE STREQUAL "stable" AND NOT PROTOCOL_VERSION STREQUAL "")
        string(APPEND PROTOCOL_INSTALL_DIR "/${PROTOCOL_VERSION}")
    endif ()

    set(BASE_NAME ${PROTOCOL_NAME})
    if (PROTOCOL_TYPE STREQUAL "unstable")
        string(APPEND BASE_NAME "-unstable")
    endif ()
    if (NOT PROTOCOL_TYPE STREQUAL "stable" AND NOT PROTOCOL_VERSION STREQUAL "")
        string(APPEND BASE_NAME "-${PROTOCOL_VERSION}")
    endif ()

    # generate protocol files by wayland-scanner
    file(MAKE_DIRECTORY ${PROTOCOL_INSTALL_DIR})
    wayland_scanner("" "client-header" ${PROTOCOL_XML_PATH} "${PROTOCOL_INSTALL_DIR}/${BASE_NAME}-client-protocol.h")
    wayland_scanner("" "server-header" ${PROTOCOL_XML_PATH} "${PROTOCOL_INSTALL_DIR}/${BASE_NAME}-server-protocol.h")
    wayland_scanner("" "public-code" ${PROTOCOL_XML_PATH} "${PROTOCOL_INSTALL_DIR}/${BASE_NAME}-protocol.c")

    # create interface target for other package to use
    # name rule: wayland-protocols-[protocol_name]-[protocol_type]-[protocol_version]
    set(TARGET_NAME "wayland-protocols-${PROTOCOL_NAME}")
    if (PROTOCOL_TYPE STREQUAL "unstable")
        string(APPEND TARGET_NAME "-${PROTOCOL_TYPE}")
    endif ()
    if (NOT PROTOCOL_TYPE STREQUAL "stable" AND NOT PROTOCOL_VERSION STREQUAL "")
        string(APPEND TARGET_NAME "-${PROTOCOL_VERSION}")
    endif ()

    # NOT all symbols will be resolved,
    # so dependencies need to be managed manually when linking these libraries.
    add_library(${TARGET_NAME} STATIC ${PROTOCOL_INSTALL_DIR}/${BASE_NAME}-protocol.c)
    target_link_libraries(${TARGET_NAME} PRIVATE wayland-util wayland-protocol-core)
    target_include_directories(${TARGET_NAME} PUBLIC ${PROTOCOL_INSTALL_DIR})
endforeach ()