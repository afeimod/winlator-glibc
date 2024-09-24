cmake_minimum_required(VERSION 3.22.1)
project(cairo)

set(CAIRO_PACKAGE "libcairo")
set(CAIRO_VERSION "1.18.2")

set(CAIRO_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(CAIRO_SRC_DIR ${CAIRO_DIR}/repo/src)
set(CAIRO_TEST_DIR ${CAIRO_DIR}/repo/meson-cc-tests)
set(AUTOGEN_DIR ${CAIRO_DIR}/autogen)

set(CAIRO_CONFIG_H_IN ${CAIRO_DIR}/config.h.in)
set(CAIRO_CONFIG_H_DEST ${AUTOGEN_DIR}/config.h)

set(CAIRO_FEATURES_H_IN ${CAIRO_DIR}/cairo-features.h.in)
set(CAIRO_FEATURES_H_DEST ${AUTOGEN_DIR}/cairo-features.h)

include(CheckFunctionExists)
include(CheckIncludeFile)
include(CheckCSourceCompiles)
include(CheckTypeSize)

# Check if need to update submodule
check_repo(${CAIRO_DIR}/repo)

# Generate config.h
set(HAVE_MKDIR "0")
set(HAS_PIXMAN_GLYPHS 1)
set(HAS_PIXMAN_r8g8b8_sRGB 1)
check_type_size("void*" SIZEOF_VOID_P)
check_type_size("int" SIZEOF_INT)
check_type_size("long" SIZEOF_LONG)
check_type_size("long long" SIZEOF_LONG_LONG)
check_type_size("size_t" SIZEOF_SIZE_T)
check_type_size("uint64_t" HAVE_UINT64_T)
check_type_size("uint128_t" HAVE_UINT128_T)
check_type_size("__uint128_t" HAVE___UINT128_T)
check_include_file("stdint.h" HAVE_STDINT_H)
check_include_file("inttypes.h" HAVE_INTTYPES_H)
check_include_file("sys/int_types.h" HAVE_SYS_INT_TYPES_H)
check_include_file("fcntl.h" HAVE_FCNTL_H)
check_include_file("unistd.h" HAVE_UNISTD_H)
check_include_file("sys/stat.h" HAVE_SYS_STAT_H)
check_include_file("sys/socket.h" HAVE_SYS_SOCKET_H)
check_include_file("poll.h" HAVE_POLL_H)
check_include_file("sys/poll.h" HAVE_SYS_POLL_H)
check_include_file("sys/un.h" HAVE_SYS_UN_H)
check_include_file("sched.h" HAVE_SCHED_H)
check_include_file("sys/mman.h" HAVE_SYS_MMAN_H)
check_include_file("time.h" HAVE_TIME_H)
check_include_file("libgen.h" HAVE_LIBGEN_H)
check_include_file("byteswap.h" HAVE_BYTESWAP_H)
check_include_file("signal.h" HAVE_SIGNAL_H)
check_include_file("setjmp.h" HAVE_SETJMP_H)
check_include_file("fenv.h" HAVE_FENV_H)
check_include_file("sys/wait.h" HAVE_SYS_WAIT_H)
check_include_file("sys/stat.h" HAVE_SYS_STAT_H)
check_include_file("io.h" HAVE_IO_H)
check_include_file("fenv.h" HAVE_FENV_H)
check_include_file("xlocale.h" HAVE_XLOCALE_H)
check_include_file("sys/ioctl.h" HAVE_SYS_IOCTL_H)
check_include_file("intsafe.h" HAVE_INTSAFE_H)
check_include_file("alloca.h" HAVE_ALLOCA_H)
check_function_exists("sched_getaffinity" HAVE_SCHED_GETAFFINITY)
check_function_exists("mmap" HAVE_MMAP)
check_function_exists("clock_gettime" HAVE_CLOCK_GETTIME)
check_function_exists("feenableexcept" HAVE_FEENABLEEXCEPT)
check_function_exists("fedisableexcept" HAVE_FEDISABLEEXCEPT)
check_function_exists("feclearexcept" HAVE_FECLEAREXCEPT)
check_function_exists("strndup" HAVE_STRNDUP)
file(READ "${CAIRO_TEST_DIR}/pthread.c" SOURCE_CODE)
check_c_source_compiles("${SOURCE_CODE}" CAIRO_HAS_PTHREAD)
if (DEFINED CAIRO_HAS_PTHREAD)
    set(CAIRO_HAS_REAL_PTHREAD 1)
endif ()
file(READ "${CAIRO_TEST_DIR}/atomic-ops-c11.c" SOURCE_CODE)
check_c_source_compiles("${SOURCE_CODE}" HAVE_C11_ATOMIC_PRIMITIVES)
check_function_exists("dlsym" CAIRO_HAS_DLSYM)
configure_file(
        ${CAIRO_CONFIG_H_IN}
        ${CAIRO_CONFIG_H_DEST}
        @ONLY
)

# Generate cairo-features.h
set(CAIRO_HAS_IMAGE_SURFACE 1)
set(CAIRO_FEATURES_H 1)
set(CAIRO_HAS_USER_FONT 1)
set(CAIRO_HAS_MIME_SURFACE 1)
set(CAIRO_HAS_RECORDING_SURFACE 1)
set(CAIRO_HAS_OBSERVER_SURFACE 1)
set(CAIRO_HAS_SVG_SURFACE 1)
set(CAIRO_HAS_PNG_FUNCTIONS 1)
configure_file(
        ${CAIRO_FEATURES_H_IN}
        ${CAIRO_FEATURES_H_DEST}
        @ONLY
)

# C args
add_compile_options(
        -O2
        -Wmissing-declarations
        -Werror-implicit-function-declaration
        -Wpointer-arith
        -Wwrite-strings
        -Wsign-compare
        -Wpacked
        -Wswitch-enum
        -Wmissing-format-attribute
        -Wvolatile-register-var
        -Wstrict-aliasing=2
        -Winit-self
        -Wno-missing-field-initializers
        -Wno-unused-parameter
        -Wno-attributes
        -Wno-long-long
        -Winline
        -Wno-unused-but-set-variable
        -Wno-enum-conversion
        -fno-strict-aliasing
        -fno-common
        -D_FORTIFY_SOURCE=2
        -D_GNU_SOURCE
        -Wno-attributes
        -Wno-unused-but-set-variable
        -Wno-missing-field-initializers
        -Wno-unused-parameter
        -Wno-long-long
        -DCAIRO_COMPILATION
)

# libcairo
add_library(
        cairo SHARED
        ${CAIRO_SRC_DIR}/cairo-analysis-surface.c
        ${CAIRO_SRC_DIR}/cairo-arc.c
        ${CAIRO_SRC_DIR}/cairo-array.c
        ${CAIRO_SRC_DIR}/cairo-atomic.c
        ${CAIRO_SRC_DIR}/cairo-base64-stream.c
        ${CAIRO_SRC_DIR}/cairo-base85-stream.c
        ${CAIRO_SRC_DIR}/cairo-bentley-ottmann-rectangular.c
        ${CAIRO_SRC_DIR}/cairo-bentley-ottmann-rectilinear.c
        ${CAIRO_SRC_DIR}/cairo-bentley-ottmann.c
        ${CAIRO_SRC_DIR}/cairo-botor-scan-converter.c
        ${CAIRO_SRC_DIR}/cairo-boxes-intersect.c
        ${CAIRO_SRC_DIR}/cairo-boxes.c
        ${CAIRO_SRC_DIR}/cairo-cache.c
        ${CAIRO_SRC_DIR}/cairo-clip-boxes.c
        ${CAIRO_SRC_DIR}/cairo-clip-polygon.c
        ${CAIRO_SRC_DIR}/cairo-clip-region.c
        ${CAIRO_SRC_DIR}/cairo-clip-surface.c
        ${CAIRO_SRC_DIR}/cairo-clip-tor-scan-converter.c
        ${CAIRO_SRC_DIR}/cairo-clip.c
        ${CAIRO_SRC_DIR}/cairo-color.c
        ${CAIRO_SRC_DIR}/cairo-composite-rectangles.c
        ${CAIRO_SRC_DIR}/cairo-compositor.c
        ${CAIRO_SRC_DIR}/cairo-contour.c
        ${CAIRO_SRC_DIR}/cairo-damage.c
        ${CAIRO_SRC_DIR}/cairo-debug.c
        ${CAIRO_SRC_DIR}/cairo-default-context.c
        ${CAIRO_SRC_DIR}/cairo-device.c
        ${CAIRO_SRC_DIR}/cairo-error.c
        ${CAIRO_SRC_DIR}/cairo-fallback-compositor.c
        ${CAIRO_SRC_DIR}/cairo-fixed.c
        ${CAIRO_SRC_DIR}/cairo-font-face-twin-data.c
        ${CAIRO_SRC_DIR}/cairo-font-face-twin.c
        ${CAIRO_SRC_DIR}/cairo-font-face.c
        ${CAIRO_SRC_DIR}/cairo-font-options.c
        ${CAIRO_SRC_DIR}/cairo-freed-pool.c
        ${CAIRO_SRC_DIR}/cairo-freelist.c
        ${CAIRO_SRC_DIR}/cairo-gstate.c
        ${CAIRO_SRC_DIR}/cairo-hash.c
        ${CAIRO_SRC_DIR}/cairo-hull.c
        ${CAIRO_SRC_DIR}/cairo-image-compositor.c
        ${CAIRO_SRC_DIR}/cairo-image-info.c
        ${CAIRO_SRC_DIR}/cairo-image-source.c
        ${CAIRO_SRC_DIR}/cairo-image-surface.c
        ${CAIRO_SRC_DIR}/cairo-line.c
        ${CAIRO_SRC_DIR}/cairo-lzw.c
        ${CAIRO_SRC_DIR}/cairo-mask-compositor.c
        ${CAIRO_SRC_DIR}/cairo-matrix.c
        ${CAIRO_SRC_DIR}/cairo-mempool.c
        ${CAIRO_SRC_DIR}/cairo-mesh-pattern-rasterizer.c
        ${CAIRO_SRC_DIR}/cairo-misc.c
        ${CAIRO_SRC_DIR}/cairo-mono-scan-converter.c
        ${CAIRO_SRC_DIR}/cairo-mutex.c
        ${CAIRO_SRC_DIR}/cairo-no-compositor.c
        ${CAIRO_SRC_DIR}/cairo-observer.c
        ${CAIRO_SRC_DIR}/cairo-output-stream.c
        ${CAIRO_SRC_DIR}/cairo-paginated-surface.c
        ${CAIRO_SRC_DIR}/cairo-path-bounds.c
        ${CAIRO_SRC_DIR}/cairo-path-fill.c
        ${CAIRO_SRC_DIR}/cairo-path-fixed.c
        ${CAIRO_SRC_DIR}/cairo-path-in-fill.c
        ${CAIRO_SRC_DIR}/cairo-path-stroke-boxes.c
        ${CAIRO_SRC_DIR}/cairo-path-stroke-polygon.c
        ${CAIRO_SRC_DIR}/cairo-path-stroke-traps.c
        ${CAIRO_SRC_DIR}/cairo-path-stroke-tristrip.c
        ${CAIRO_SRC_DIR}/cairo-path-stroke.c
        ${CAIRO_SRC_DIR}/cairo-path.c
        ${CAIRO_SRC_DIR}/cairo-pattern.c
        ${CAIRO_SRC_DIR}/cairo-pen.c
        ${CAIRO_SRC_DIR}/cairo-polygon-intersect.c
        ${CAIRO_SRC_DIR}/cairo-polygon-reduce.c
        ${CAIRO_SRC_DIR}/cairo-polygon.c
        ${CAIRO_SRC_DIR}/cairo-raster-source-pattern.c
        ${CAIRO_SRC_DIR}/cairo-recording-surface.c
        ${CAIRO_SRC_DIR}/cairo-rectangle.c
        ${CAIRO_SRC_DIR}/cairo-rectangular-scan-converter.c
        ${CAIRO_SRC_DIR}/cairo-region.c
        ${CAIRO_SRC_DIR}/cairo-rtree.c
        ${CAIRO_SRC_DIR}/cairo-scaled-font.c
        ${CAIRO_SRC_DIR}/cairo-shape-mask-compositor.c
        ${CAIRO_SRC_DIR}/cairo-slope.c
        ${CAIRO_SRC_DIR}/cairo-spans-compositor.c
        ${CAIRO_SRC_DIR}/cairo-spans.c
        ${CAIRO_SRC_DIR}/cairo-spline.c
        ${CAIRO_SRC_DIR}/cairo-stroke-dash.c
        ${CAIRO_SRC_DIR}/cairo-stroke-style.c
        ${CAIRO_SRC_DIR}/cairo-surface-clipper.c
        ${CAIRO_SRC_DIR}/cairo-surface-fallback.c
        ${CAIRO_SRC_DIR}/cairo-surface-observer.c
        ${CAIRO_SRC_DIR}/cairo-surface-offset.c
        ${CAIRO_SRC_DIR}/cairo-surface-snapshot.c
        ${CAIRO_SRC_DIR}/cairo-surface-subsurface.c
        ${CAIRO_SRC_DIR}/cairo-surface-wrapper.c
        ${CAIRO_SRC_DIR}/cairo-surface.c
        ${CAIRO_SRC_DIR}/cairo-time.c
        ${CAIRO_SRC_DIR}/cairo-tor-scan-converter.c
        ${CAIRO_SRC_DIR}/cairo-tor22-scan-converter.c
        ${CAIRO_SRC_DIR}/cairo-toy-font-face.c
        ${CAIRO_SRC_DIR}/cairo-traps-compositor.c
        ${CAIRO_SRC_DIR}/cairo-traps.c
        ${CAIRO_SRC_DIR}/cairo-tristrip.c
        ${CAIRO_SRC_DIR}/cairo-unicode.c
        ${CAIRO_SRC_DIR}/cairo-user-font.c
        ${CAIRO_SRC_DIR}/cairo-version.c
        ${CAIRO_SRC_DIR}/cairo-wideint.c
        ${CAIRO_SRC_DIR}/cairo.c
        ${CAIRO_SRC_DIR}/cairo-cff-subset.c
        ${CAIRO_SRC_DIR}/cairo-scaled-font-subsets.c
        ${CAIRO_SRC_DIR}/cairo-truetype-subset.c
        ${CAIRO_SRC_DIR}/cairo-type1-fallback.c
        ${CAIRO_SRC_DIR}/cairo-type1-glyph-names.c
        ${CAIRO_SRC_DIR}/cairo-type1-subset.c
        ${CAIRO_SRC_DIR}/cairo-type3-glyph-surface.c
        ${CAIRO_SRC_DIR}/cairo-pdf-operators.c
        ${CAIRO_SRC_DIR}/cairo-pdf-shading.c
        ${CAIRO_SRC_DIR}/cairo-tag-attributes.c
        ${CAIRO_SRC_DIR}/cairo-tag-stack.c
        ${CAIRO_SRC_DIR}/cairo-deflate-stream.c
        # png
        ${CAIRO_SRC_DIR}/cairo-png.c
        # svg
        ${CAIRO_SRC_DIR}/cairo-svg-surface.c
)

target_link_libraries(
        cairo PUBLIC
        pixman
        png
)

target_include_directories(
        cairo PUBLIC
        ${CAIRO_SRC_DIR}
        ${AUTOGEN_DIR}
)



