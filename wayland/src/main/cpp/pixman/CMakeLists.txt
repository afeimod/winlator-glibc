cmake_minimum_required(VERSION 3.22.1)
project(pixman)

set(PIXMAN_PACKAGE "libpixman")
set(PIXMAN_VERSION "0.43.4")

set(PIXMAN_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(PIXMAN_SRC_DIR ${PIXMAN_DIR}/repo/pixman)
set(AUTOGEN_DIR ${PIXMAN_DIR}/autogen)

set(PIXMAN_CONFIG_H_IN ${PIXMAN_DIR}/pixman-config.h.in)
set(PIXMAN_CONFIG_H_DEST ${AUTOGEN_DIR}/pixman-config.h)

set(PIXMAN_VERSION_H_IN ${PIXMAN_SRC_DIR}/pixman-version.h.in)
set(PIXMAN_VERSION_H_DEST ${AUTOGEN_DIR}/pixman-version.h)

include(CheckFunctionExists)
include(CheckIncludeFile)
include(CheckCSourceCompiles)
include(CheckTypeSize)
include(CheckSymbolExists)
include(FindOpenMP)

enable_language(ASM)

# Check if need to update submodule
check_repo(${PIXMAN_DIR}/repo)

# Only aarch64
aarch64_only()

# Generate config.h
set(USE_LOONGSON_MMI 0)
set(USE_X86_MMX 0)
set(USE_SSE2 0)
set(USE_SSSE3 0)
set(USE_VMX 0)
set(USE_ARM_SIMD 0)
set(USE_ARM_NEON 0)
set(USE_MIPS_DSPR2 0)
set(PIXMAN_TIMERS 0)
set(PIXMAN_GNUPLOT 0)
set(HAVE_LIBPNG 0)
set(WORDS_BIGENDIAN 0)
set(PACKAGE foo)
if (DEFINED OpenMP_FOUND)
    set(USE_OPENMP 1)
else ()
    set(USE_OPENMP 0)
endif ()
check_include_file("pthread.h" HAVE_PTHREADS)
check_include_file("sys/mman.h" HAVE_SYS_MMAN_H)
check_include_file("fenv.h" HAVE_FENV_H)
check_include_file("unistd.h" HAVE_UNISTD_H)
check_function_exists(sigaction HAVE_SIGACTION)
check_function_exists(alarm HAVE_ALARM)
check_function_exists(mprotect HAVE_MPROTECT)
check_function_exists(getpagesize HAVE_GETPAGESIZE)
check_function_exists(mmap HAVE_MMAP)
check_function_exists(getisax HAVE_GETISAX)
check_function_exists(gettimeofday HAVE_GETTIMEOFDAY)
check_function_exists(posix_memalign HAVE_POSIX_MEMALIGN)
check_function_exists(clz HAVE_BUILTIN_CLZ)
check_symbol_exists(FE_DIVBYZERO "fenv.h" HAVE_FEDIVBYZERO)
check_type_size("long" SIZEOF_LONG)
check_c_source_compiles("
    __asm__ (
    \".func meson_test\"
    \".endfunc\"
    );
" ASM_HAVE_FUNC_DIRECTIVE)
check_c_source_compiles("
    __asm__ (
    \".syntax unified\n\"
    );
" ASM_HAVE_SYNTAX_UNIFIED)
check_c_source_compiles("
    #include <stdint.h>
    __asm__ (
        \"   .global _testlabel\n\"
        \"_testlabel:\n\"
    );

    int testlabel();
    int main(int argc, char* argv[]) {
        return testlabel();
    }
" ASM_LEADING_UNDERSCORE)
check_c_source_compiles("
    .text
    .arch armv8-a
    .altmacro
    prfm pldl2strm, [x0]
    xtn v0.8b, v0.8h
" USE_ARM_A64_NEON)
check_c_source_compiles("
        #ifndef __IWMMXT__
        #error \"IWMMXT not enabled (with -march=iwmmxt)\"
        #endif
        #if defined(__GNUC__) && (__GNUC__ < 4 || (__GNUC__ == 4 && __GNUC_MINOR__ < 8))
        #error \"Need GCC >= 4.8 for IWMMXT intrinsics\"
        #endif
        #include <mmintrin.h>
        int main () {
          union {
            __m64 v;
            char c[8];
          } a = { .c = {1, 2, 3, 4, 5, 6, 7, 8} };
          int b = 4;
          __m64 c = _mm_srli_si64 (a.v, b);
        }
" USE_ARM_IWMMXT)
check_c_source_compiles("
      int main () {
        /* Most modern architectures have a NOP instruction, so this is a fairly generic test. */
        asm volatile (\"nop\");
        return 0;
      }
" USE_GCC_INLINE_ASM)
check_c_source_compiles("
        #define _GNU_SOURCE
        #include <fenv.h>
        int main() {
            feenableexcept(FE_DIVBYZERO | FE_INVALID | FE_OVERFLOW);
            return 0;
        }
" HAVE_FEENABLEEXCEPT)
check_c_source_compiles("
    #include <stdio.h>
    __thread int tls_var = 0;
" TLS)
if (DEFINED TLS)
    set(TLS __thread)
endif ()
check_c_source_compiles("
    static int x = 1;
    static void __attribute__((constructor)) constructor_function () { x = 0; }
    int main (void) { return x; }
" TOOLCHAIN_SUPPORTS_ATTRIBUTE_CONSTRUCTOR)
check_c_source_compiles("
    __float128 a = 1.0Q, b = 2.0Q;
    int main (void) { return a + b; }
" HAVE_FLOAT128)
check_c_source_compiles("
    unsigned int __attribute__ ((vector_size(16))) e, a, b;
    int main (void) { e = a - ((b << 27) + (b >> (32 - 27))) + 1; return e[0]; }
" HAVE_GCC_VECTOR_EXTENSIONS)
configure_file(
        ${PIXMAN_CONFIG_H_IN}
        ${PIXMAN_CONFIG_H_DEST}
        @ONLY
)

# Generate pixman-version.h
string(REPLACE "." ";" VERSION_LIST "${PIXMAN_VERSION}")
list(GET VERSION_LIST 0 PIXMAN_VERSION_MAJOR)
list(GET VERSION_LIST 1 PIXMAN_VERSION_MINOR)
list(GET VERSION_LIST 2 PIXMAN_VERSION_MICRO)
configure_file(
        ${PIXMAN_VERSION_H_IN}
        ${PIXMAN_VERSION_H_DEST}
        @ONLY
)

# C args
add_compile_options(
        -std=gnu99
        -O2
        -Wdeclaration-after-statement
        -fno-strict-aliasing
        -fvisibility=hidden
        -Wundef
        -ftrapping-math
        -Wno-unused-local-typedefs
        -DHAVE_CONFIG_H
)

# pixman-arm-neon
add_library(
        pixman-arm-neon STATIC
        ${PIXMAN_SRC_DIR}/pixman-arma64-neon-asm.S
        ${PIXMAN_SRC_DIR}/pixman-arma64-neon-asm-bilinear.S
)

target_include_directories(pixman-arm-neon PRIVATE ${AUTOGEN_DIR})

# libpixman
add_library(
        pixman SHARED
        ${PIXMAN_SRC_DIR}/pixman.c
        ${PIXMAN_SRC_DIR}/pixman-access.c
        ${PIXMAN_SRC_DIR}/pixman-access-accessors.c
        ${PIXMAN_SRC_DIR}/pixman-bits-image.c
        ${PIXMAN_SRC_DIR}/pixman-combine32.c
        ${PIXMAN_SRC_DIR}/pixman-combine-float.c
        ${PIXMAN_SRC_DIR}/pixman-conical-gradient.c
        ${PIXMAN_SRC_DIR}/pixman-filter.c
        ${PIXMAN_SRC_DIR}/pixman-x86.c
        ${PIXMAN_SRC_DIR}/pixman-mips.c
        ${PIXMAN_SRC_DIR}/pixman-arm.c
        ${PIXMAN_SRC_DIR}/pixman-ppc.c
        ${PIXMAN_SRC_DIR}/pixman-edge.c
        ${PIXMAN_SRC_DIR}/pixman-edge-accessors.c
        ${PIXMAN_SRC_DIR}/pixman-fast-path.c
        ${PIXMAN_SRC_DIR}/pixman-glyph.c
        ${PIXMAN_SRC_DIR}/pixman-general.c
        ${PIXMAN_SRC_DIR}/pixman-gradient-walker.c
        ${PIXMAN_SRC_DIR}/pixman-image.c
        ${PIXMAN_SRC_DIR}/pixman-implementation.c
        ${PIXMAN_SRC_DIR}/pixman-linear-gradient.c
        ${PIXMAN_SRC_DIR}/pixman-matrix.c
        ${PIXMAN_SRC_DIR}/pixman-noop.c
        ${PIXMAN_SRC_DIR}/pixman-radial-gradient.c
        ${PIXMAN_SRC_DIR}/pixman-region16.c
        ${PIXMAN_SRC_DIR}/pixman-region32.c
        ${PIXMAN_SRC_DIR}/pixman-solid-fill.c
        ${PIXMAN_SRC_DIR}/pixman-timer.c
        ${PIXMAN_SRC_DIR}/pixman-trap.c
        ${PIXMAN_SRC_DIR}/pixman-utils.c
)

target_link_libraries(pixman PRIVATE pixman-arm-neon)

target_include_directories(pixman PUBLIC ${PIXMAN_SRC_DIR})

target_include_directories(pixman PRIVATE ${AUTOGEN_DIR})