cmake_minimum_required(VERSION 3.22.1)
project(xkbcommon)

set(XKBCOMMON_VERSION "1.7.0")

set(XKBCOMMON_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(XKBCOMMON_SRC_DIR ${XKBCOMMON_DIR}/repo/src)
set(XKBCOMMON_INCLUDE_DIR ${XKBCOMMON_DIR}/repo/include)
set(XKBCOMMON_PARSER_DIR ${XKBCOMMON_DIR}/parser)
set(AUTOGEN_DIR ${XKBCOMMON_DIR}/autogen)

set(CONFIG_H_IN ${XKBCOMMON_DIR}/config.h.in)
set(CONFIG_H_DEST ${AUTOGEN_DIR}/config.h)

include(CheckFunctionExists)
include(CheckIncludeFile)
include(CheckCSourceCompiles)
include(CheckSymbolExists)

# Check if need to update submodule
check_repo(${XKBCOMMON_DIR}/repo)

# Generate config.h
set(EXIT_INVALID_USAGE 2)
set(LIBXKBCOMMON_VERSION ${XKBCOMMON_VERSION})
set(LIBXKBCOMMON_TOOL_PATH 0)
set(_GNU_SOURCE 1)
set(DFLT_XKB_CONFIG_ROOT " ")
set(DFLT_XKB_CONFIG_EXTRA_PATH " ")
set(XLOCALEDIR " ")
set(DEFAULT_XKB_RULES " ")
set(DEFAULT_XKB_MODEL " ")
set(DEFAULT_XKB_LAYOUT " ")
set(DEFAULT_XKB_VARIANT " ")
set(DEFAULT_XKB_OPTIONS " ")
set(_CRT_SECURE_NO_WARNINGS 1)
set(_CRT_NONSTDC_NO_WARNINGS 1)
set(_CRT_NONSTDC_NO_DEPRECATE 1)
set(WIN32_LEAN_AND_MEAN 1)
check_include_file(unistd.h HAVE_UNISTD_H)
check_symbol_exists(eaccess "unistd.h" HAVE_EACCESS)
check_symbol_exists(euidaccess "unistd.h" HAVE_EUIDACCESS)
check_symbol_exists(mmap "sys/mman.h" HAVE_MMAP)
check_symbol_exists(mkostemp "stdlib.h" HAVE_MKOSTEMP)
check_symbol_exists(posix_fallocate "fcntl.h" HAVE_POSIX_FALLOCATE)
check_symbol_exists(strndup "string.h" HAVE_STRNDUP)
check_symbol_exists(asprintf "stdio.h" HAVE_ASPRINTF)
check_symbol_exists(vasprintf "stdio.h" HAVE_VASPRINTF)
check_symbol_exists(secure_getenv "stdlib.h" HAVE_SECURE_GETENV)
if (NOT DEFINED HAVE_SECURE_GETENV)
    check_symbol_exists(__secure_getenv "stdlib.h" HAVE___SECURE_GETENV)
endif ()
check_symbol_exists(PATH_MAX "limits.h" HAVE_PATH_MAX)
if (DEFINED HAVE_PATH_MAX)
    set(PATH_MAX 4096)
endif ()
check_c_source_compiles("
        int main(){if(__builtin_expect(1<0,0)){}}
" HAVE___BUILTIN_EXPECT)
configure_file(
        ${CONFIG_H_IN}
        ${CONFIG_H_DEST}
        @ONLY
)

# C args
add_compile_options(
        -O2
        -std=c11
        -fno-strict-aliasing
        -Wno-unused-parameter
        -Wno-missing-field-initializers
        -Wpointer-arith
        -Wmissing-declarations
        -Wformat=2
        -Wstrict-prototypes
        -Wmissing-prototypes
        -Wnested-externs
        -Wbad-function-cast
        -Wshadow
        -Wlogical-op
        -Wdate-time
        -Wwrite-strings
        -Wno-documentation-deprecated-sync
)

# libxkbcommon
add_library(
        xkbcommon SHARED
        ${XKBCOMMON_SRC_DIR}/compose/parser.c
        ${XKBCOMMON_SRC_DIR}/compose/paths.c
        ${XKBCOMMON_SRC_DIR}/compose/state.c
        ${XKBCOMMON_SRC_DIR}/compose/table.c
        ${XKBCOMMON_SRC_DIR}/xkbcomp/action.c
        ${XKBCOMMON_SRC_DIR}/xkbcomp/ast-build.c
        ${XKBCOMMON_SRC_DIR}/xkbcomp/compat.c
        ${XKBCOMMON_SRC_DIR}/xkbcomp/expr.c
        ${XKBCOMMON_SRC_DIR}/xkbcomp/include.c
        ${XKBCOMMON_SRC_DIR}/xkbcomp/keycodes.c
        ${XKBCOMMON_SRC_DIR}/xkbcomp/keymap.c
        ${XKBCOMMON_SRC_DIR}/xkbcomp/keymap-dump.c
        ${XKBCOMMON_SRC_DIR}/xkbcomp/keywords.c
        ${XKBCOMMON_PARSER_DIR}/parser.c
        ${XKBCOMMON_SRC_DIR}/xkbcomp/rules.c
        ${XKBCOMMON_SRC_DIR}/xkbcomp/scanner.c
        ${XKBCOMMON_SRC_DIR}/xkbcomp/symbols.c
        ${XKBCOMMON_SRC_DIR}/xkbcomp/types.c
        ${XKBCOMMON_SRC_DIR}/xkbcomp/vmod.c
        ${XKBCOMMON_SRC_DIR}/xkbcomp/xkbcomp.c
        ${XKBCOMMON_SRC_DIR}/atom.c
        ${XKBCOMMON_SRC_DIR}/context.c
        ${XKBCOMMON_SRC_DIR}/context-priv.c
        ${XKBCOMMON_SRC_DIR}/keysym.c
        ${XKBCOMMON_SRC_DIR}/keysym-utf.c
        ${XKBCOMMON_SRC_DIR}/keymap.c
        ${XKBCOMMON_SRC_DIR}/keymap-priv.c
        ${XKBCOMMON_SRC_DIR}/state.c
        ${XKBCOMMON_SRC_DIR}/text.c
        ${XKBCOMMON_SRC_DIR}/utf8.c
        ${XKBCOMMON_SRC_DIR}/utils.c
)

target_include_directories(xkbcommon PUBLIC ${XKBCOMMON_INCLUDE_DIR})

target_include_directories(
        xkbcommon PRIVATE
        ${XKBCOMMON_SRC_DIR}
        ${AUTOGEN_DIR}
        ${XKBCOMMON_PARSER_DIR}
)

