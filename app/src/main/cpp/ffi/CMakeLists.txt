cmake_minimum_required(VERSION 3.22.1)
project(ffi)

set(FFI_PACKAGE "libffi")
set(FFI_VERSION "3.4.6")
set(FFI_DEBUG OFF)

set(FFI_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(FFI_SRC_DIR ${FFI_DIR}/repo/src)
set(FFI_INCLUDE_DIR ${FFI_DIR}/repo/include)
set(AUTOGEN_DIR ${FFI_DIR}/autogen)

set(FFICONFIG_H_IN ${FFI_DIR}/fficonfig.h.in)
set(FFICONFIG_H_DEST ${AUTOGEN_DIR}/config/fficonfig.h)

set(FFI_H_IN ${FFI_INCLUDE_DIR}/ffi.h.in)
set(FFI_H_DEST ${AUTOGEN_DIR}/include/ffi.h)

include(CheckFunctionExists)
include(CheckIncludeFile)
include(CheckCSourceCompiles)
include(CheckTypeSize)

enable_language(ASM)

# Check if need to update submodule
check_repo(${FFI_DIR}/repo)

# Only aarch64
aarch64_only()

# Check
set(EH_FRAME_FLAGS "a")
set(HAVE_LONG_DOUBLE 1)
set(HAVE_RO_EH_FRAME 1)
set(LT_OBJDIR ".libs/")
set(PACKAGE ${FFI_PACKAGE})
set(PACKAGE_BUGREPORT "http://github.com/libffi/libffi/issues")
set(PACKAGE_NAME ${FFI_PACKAGE})
set(PACKAGE_STRING "${FFI_PACKAGE} ${FFI_VERSION}")
set(PACKAGE_TARNAME ${FFI_PACKAGE})
set(PACKAGE_URL "")
set(PACKAGE_VERSION ${FFI_VERSION})
set(STDC_HEADERS 1)
set(VERSION ${FFI_VERSION})
check_type_size("double" SIZEOF_DOUBLE)
check_type_size("long double" SIZEOF_LONG_DOUBLE)
check_type_size("size_t" SIZEOF_SIZE_T)
check_include_file("alloca.h" HAVE_ALLOCA_H)
check_include_file("dlfcn.h" HAVE_DLFCN_H)
check_include_file("inttypes.h" HAVE_INTTYPES_H)
check_include_file("stdint.h" HAVE_STDINT_H)
check_include_file("stdio.h" HAVE_STDIO_H)
check_include_file("stdlib.h" HAVE_STDLIB_H)
check_include_file("strings.h" HAVE_STRINGS_H)
check_include_file("string.h" HAVE_STRING_H)
check_include_file("sys/memfd.h" HAVE_SYS_MEMFD_H)
check_include_file("sys/stat.h" HAVE_SYS_STAT_H)
check_include_file("sys/types.h" HAVE_SYS_TYPES_H)
check_include_file("unistd.h" HAVE_UNISTD_H)
check_include_file("alloca.h" HAVE_ALLOCA_H)
check_include_file("alloca.h" HAVE_ALLOCA_H)
check_include_file("alloca.h" HAVE_ALLOCA_H)
check_function_exists(memcpy HAVE_MEMCPY)
check_function_exists(memfd_create HAVE_MEMFD_CREATE)
check_c_source_compiles("
    .section .text
    .globl _start
    _start:
        .cfi_startproc
        nop
        .cfi_endproc
" HAVE_AS_CFI_PSEUDO_OP)
check_c_source_compiles("
    .section .text
    .globl _start
    _start:
        .register x0, x1, x2, x3
        nop
" HAVE_AS_REGISTER_PSEUDO_OP)
check_c_source_compiles("
    __attribute__((visibility(\"hidden\"))) void test_hidden_visibility(void);
    void test_hidden_visibility(void) {}
    int main(int argc, char** argv) {
        return 0;
    }
" HAVE_HIDDEN_VISIBILITY_ATTRIBUTE)
check_c_source_compiles("
    .section .text
    .global _start
    _start:
        pacia x0, x1
        autia x0, x1
        pacib x0, x1
        autib x0, x1
        blraaz x0
        retab x0
        nop
" HAVE_PTRAUTH)
check_c_source_compiles("
    #include <features.h>
    #if defined(__GNUC__) && defined(_GNU_SOURCE) && defined(__VERSION__)
        int main(int argc, char** argv) { return 0; }
    #else
        #error GNU symbol versioning is not supported
    #endif
" LIBFFI_GNU_SYMBOL_VERSIONING)

set(TARGET "AARCH64")
set(FFI_EXEC_TRAMPOLINE_TABLE 0)
check_c_source_compiles("
        int main(int argc, char** argv) {
            long double ld = 1.0;
            int size = sizeof(ld);
            return 0;
        }
" HAVE_LONG_DOUBLE)

file(MAKE_DIRECTORY ${AUTOGEN_DIR})
file(MAKE_DIRECTORY ${AUTOGEN_DIR}/include)
file(MAKE_DIRECTORY ${AUTOGEN_DIR}/config)

# Generate fficonfig.h
configure_file(
        ${FFICONFIG_H_IN}
        ${FFICONFIG_H_DEST}
        @ONLY
)

# Generate ffi.h
configure_file(
        ${FFI_H_IN}
        ${FFI_H_DEST}
        @ONLY
)

# Install ffitarget.h
configure_file(
        ${FFI_SRC_DIR}/aarch64/ffitarget.h
        ${AUTOGEN_DIR}/include/
        COPYONLY
)

# C args
add_compile_options(
        -O3
        -fomit-frame-pointer
        -fstrict-aliasing
        -ffast-math
        -fexceptions
        -Wno-deprecated-declarations
)

# libffi
add_library(
        ffi SHARED
        ${FFI_SRC_DIR}/closures.c
        ${FFI_SRC_DIR}/debug.c
        ${FFI_SRC_DIR}/java_raw_api.c
        ${FFI_SRC_DIR}/prep_cif.c
        ${FFI_SRC_DIR}/raw_api.c
        ${FFI_SRC_DIR}/tramp.c
        ${FFI_SRC_DIR}/types.c
        ${FFI_SRC_DIR}/aarch64/ffi.c
        ${FFI_SRC_DIR}/aarch64/sysv.S
)

target_include_directories(
        ffi PUBLIC
        ${AUTOGEN_DIR}/include
)

target_include_directories(
        ffi PRIVATE
        ${AUTOGEN_DIR}/config
        ${FFI_SRC_DIR}/aarch64
        ${FFI_INCLUDE_DIR}
)