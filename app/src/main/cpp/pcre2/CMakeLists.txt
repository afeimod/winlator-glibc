cmake_minimum_required(VERSION 3.22.1)
project(pcre2)

set(PCRE2_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(PCRE2_SRC_DIR ${PCRE2_DIR}/repo/src)
set(AUTOGEN_DIR ${PCRE2_DIR}/autogen)

set(CONFIG_H_IN ${PCRE2_DIR}/repo/config-cmake.h.in)
set(CONFIG_H_DEST ${AUTOGEN_DIR}/config/config.h)

set(PCRE2_H_IN ${PCRE2_SRC_DIR}/pcre2.h.in)
set(PCRE2_H_DEST ${AUTOGEN_DIR}/include/pcre2.h)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED TRUE)
set(CMAKE_C_VISIBILITY_PRESET hidden)

include(CheckCSourceCompiles)
include(CheckSymbolExists)
include(CheckIncludeFile)

# Check if need to update submodule
check_repo(${PCRE2_DIR}/repo)

# Check C
check_include_file(dirent.h HAVE_DIRENT_H)
check_include_file(sys/stat.h HAVE_SYS_STAT_H)
check_include_file(sys/types.h HAVE_SYS_TYPES_H)
check_include_file(unistd.h HAVE_UNISTD_H)
check_include_file(windows.h HAVE_WINDOWS_H)
check_symbol_exists(bcopy "strings.h" HAVE_BCOPY)
check_symbol_exists(memfd_create "sys/mman.h" HAVE_MEMFD_CREATE)
check_symbol_exists(memmove "string.h" HAVE_MEMMOVE)
check_symbol_exists(secure_getenv "stdlib.h" HAVE_SECURE_GETENV)
check_symbol_exists(strerror "string.h" HAVE_STRERROR)
check_c_source_compiles("
    #include <stdlib.h>
    #include <limits.h>
    int main(int c, char *v[]) { char buf[PATH_MAX]; realpath(v[c], buf); return 0; }
" HAVE_REALPATH)
set(ORIG_CMAKE_REQUIRED_FLAGS ${CMAKE_REQUIRED_FLAGS})
set(CMAKE_REQUIRED_FLAGS "${CMAKE_REQUIRED_FLAGS} -Werror")
check_c_source_compiles("
    #include <stddef.h>
    int main(void) { int a,b; size_t m; __builtin_mul_overflow(a,b,&m); return 0; }
" HAVE_BUILTIN_MUL_OVERFLOW)
check_c_source_compiles("
    int main(void) { char buf[128] __attribute__((uninitialized)); (void)buf; return 0; }
" HAVE_ATTRIBUTE_UNINITIALIZED)
check_c_source_compiles([=[
  extern __attribute__ ((visibility ("default"))) int f(void);
  int main(void) { return f(); }
  int f(void) { return 42; }
  ]=] HAVE_VISIBILITY
)
if (HAVE_VISIBILITY)
    set(PCRE2_EXPORT [=[__attribute__ ((visibility ("default")))]=])
else()
    set(PCRE2_EXPORT)
endif()
set(CMAKE_REQUIRED_FLAGS ${ORIG_CMAKE_REQUIRED_FLAGS})

set(SUPPORT_PCRE2_8 1)
set(PCRE2_LINK_SIZE "2")
set(PCRE2_PARENS_NEST_LIMIT "250")
set(PCRE2_HEAP_LIMIT "20000000")
set(PCRE2_MAX_VARLOOKBEHIND "255")
set(PCRE2_MATCH_LIMIT "10000000")
set(PCRE2_MATCH_LIMIT_DEPTH "MATCH_LIMIT")
set(PCRE2GREP_BUFSIZE "20480")
set(PCRE2GREP_MAX_BUFSIZE "1048576")
set(NEWLINE_DEFAULT "2")
set(SUPPORT_PCRE2GREP_JIT 1)
set(SUPPORT_PCRE2GREP_CALLOUT 1)
set(SUPPORT_PCRE2GREP_CALLOUT_FORK 1)
set(SUPPORT_UNICODE 1)
set(SUPPORT_LIBZ 1)

# Generate config.h
configure_file(
        ${CONFIG_H_IN}
        ${CONFIG_H_DEST}
        @ONLY
)

# Parse version numbers and date out of configure.ac
file(STRINGS ${PCRE2_DIR}/repo/configure.ac
        configure_lines
        LIMIT_COUNT 50 # Read only the first 50 lines of the file
)

set(SEARCHED_VARIABLES "pcre2_major" "pcre2_minor" "pcre2_prerelease" "pcre2_date"
        "libpcre2_posix_version" "libpcre2_8_version" "libpcre2_16_version" "libpcre2_32_version")
foreach(configure_line ${configure_lines})
    foreach(_substitution_variable ${SEARCHED_VARIABLES})
        string(TOUPPER ${_substitution_variable} _substitution_variable_upper)
        if (NOT ${_substitution_variable_upper})
            string(REGEX MATCH "m4_define\\(${_substitution_variable}, *\\[(.*)\\]" MATCHED_STRING ${configure_line})
            if (CMAKE_MATCH_1)
                set(${_substitution_variable_upper} ${CMAKE_MATCH_1})
            endif()
        endif()
    endforeach()
endforeach()

macro(PARSE_LIB_VERSION VARIABLE_PREFIX)
    string(REPLACE ":" ";" ${VARIABLE_PREFIX}_VERSION_LIST ${${VARIABLE_PREFIX}_VERSION})
    list(GET ${VARIABLE_PREFIX}_VERSION_LIST 0 ${VARIABLE_PREFIX}_VERSION_CURRENT)
    list(GET ${VARIABLE_PREFIX}_VERSION_LIST 1 ${VARIABLE_PREFIX}_VERSION_REVISION)
    list(GET ${VARIABLE_PREFIX}_VERSION_LIST 2 ${VARIABLE_PREFIX}_VERSION_AGE)

    math(EXPR ${VARIABLE_PREFIX}_SOVERSION "${${VARIABLE_PREFIX}_VERSION_CURRENT} - ${${VARIABLE_PREFIX}_VERSION_AGE}")
    math(EXPR ${VARIABLE_PREFIX}_MACHO_COMPATIBILITY_VERSION "${${VARIABLE_PREFIX}_VERSION_CURRENT} + 1")
    math(EXPR ${VARIABLE_PREFIX}_MACHO_CURRENT_VERSION "${${VARIABLE_PREFIX}_VERSION_CURRENT} + 1")
    set(${VARIABLE_PREFIX}_MACHO_CURRENT_VERSION "${${VARIABLE_PREFIX}_MACHO_CURRENT_VERSION}.${${VARIABLE_PREFIX}_VERSION_REVISION}}")
    set(${VARIABLE_PREFIX}_VERSION "${${VARIABLE_PREFIX}_SOVERSION}.${${VARIABLE_PREFIX}_VERSION_AGE}.${${VARIABLE_PREFIX}_VERSION_REVISION}")
endmacro()

PARSE_LIB_VERSION(LIBPCRE2_POSIX)
PARSE_LIB_VERSION(LIBPCRE2_8)
PARSE_LIB_VERSION(LIBPCRE2_16)
PARSE_LIB_VERSION(LIBPCRE2_32)

FILE(MAKE_DIRECTORY ${AUTOGEN_DIR})
FILE(MAKE_DIRECTORY ${AUTOGEN_DIR}/config)
FILE(MAKE_DIRECTORY ${AUTOGEN_DIR}/include)
FILE(MAKE_DIRECTORY ${AUTOGEN_DIR}/src)

# Generate pcre2.h
configure_file(
        ${PCRE2_H_IN}
        ${PCRE2_H_DEST}
        @ONLY
)

# Generate pcre2_chartables.c
configure_file(
        ${PCRE2_SRC_DIR}/pcre2_chartables.c.dist
        ${AUTOGEN_DIR}/src/pcre2_chartables.c
        COPYONLY
)

# C args
add_compile_options(
        -O2
)

# libpcre2-8
add_library(
        pcre2-8 SHARED
        ${PCRE2_SRC_DIR}/pcre2_auto_possess.c
        ${AUTOGEN_DIR}/src/pcre2_chartables.c
        ${PCRE2_SRC_DIR}/pcre2_chkdint.c
        ${PCRE2_SRC_DIR}/pcre2_compile.c
        ${PCRE2_SRC_DIR}/pcre2_config.c
        ${PCRE2_SRC_DIR}/pcre2_context.c
        ${PCRE2_SRC_DIR}/pcre2_convert.c
        ${PCRE2_SRC_DIR}/pcre2_dfa_match.c
        ${PCRE2_SRC_DIR}/pcre2_error.c
        ${PCRE2_SRC_DIR}/pcre2_extuni.c
        ${PCRE2_SRC_DIR}/pcre2_find_bracket.c
        ${PCRE2_SRC_DIR}/pcre2_jit_compile.c
        ${PCRE2_SRC_DIR}/pcre2_maketables.c
        ${PCRE2_SRC_DIR}/pcre2_match.c
        ${PCRE2_SRC_DIR}/pcre2_match_data.c
        ${PCRE2_SRC_DIR}/pcre2_newline.c
        ${PCRE2_SRC_DIR}/pcre2_ord2utf.c
        ${PCRE2_SRC_DIR}/pcre2_pattern_info.c
        ${PCRE2_SRC_DIR}/pcre2_script_run.c
        ${PCRE2_SRC_DIR}/pcre2_serialize.c
        ${PCRE2_SRC_DIR}/pcre2_string_utils.c
        ${PCRE2_SRC_DIR}/pcre2_study.c
        ${PCRE2_SRC_DIR}/pcre2_substitute.c
        ${PCRE2_SRC_DIR}/pcre2_substring.c
        ${PCRE2_SRC_DIR}/pcre2_tables.c
        ${PCRE2_SRC_DIR}/pcre2_ucd.c
        ${PCRE2_SRC_DIR}/pcre2_valid_utf.c
        ${PCRE2_SRC_DIR}/pcre2_xclass.c
)

target_include_directories(
        pcre2-8 PRIVATE
        ${PCRE2_SRC_DIR}
        ${AUTOGEN_DIR}/config
)

target_include_directories(
        pcre2-8 PUBLIC
        ${AUTOGEN_DIR}/include
)

target_compile_definitions(
        pcre2-8 PUBLIC
        -DHAVE_CONFIG_H
        -DPCRE2_CODE_UNIT_WIDTH=8
)

# libpcre2-posix
add_library(
        pcre2-posix SHARED
        ${PCRE2_SRC_DIR}/pcre2posix.c
)

target_include_directories(
        pcre2-posix PRIVATE
        ${AUTOGEN_DIR}/config
)

target_compile_definitions(
        pcre2-posix PUBLIC
        -DPCRE2POSIX_SHARED
)

target_link_libraries(
        pcre2-posix PUBLIC
        pcre2-8
)