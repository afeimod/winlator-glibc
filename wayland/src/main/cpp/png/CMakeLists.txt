cmake_minimum_required(VERSION 3.22.1)
project(png)

set(PNG_PACKAGE "libpng")
set(PNG_VERSION "1.6.44")

set(PNG_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(PNG_SRC_DIR ${PNG_DIR}/repo)
set(AUTOGEN_DIR ${PNG_DIR}/autogen)

set(CONFIG_H_IN ${PNG_DIR}/config.h.in)
set(CONFIG_H_DEST ${AUTOGEN_DIR}/config.h)

set(PNGLIBCONF_H_PREBUILT ${PNG_SRC_DIR}/scripts/pnglibconf.h.prebuilt)
set(PNGLIBCONF_H_DEST ${AUTOGEN_DIR}/pnglibconf.h)

include(CheckFunctionExists)
include(CheckIncludeFile)
include(CheckCSourceCompiles)
include(CheckSymbolExists)
include(CheckLinkerFlag)

enable_language(ASM)

# Check if need to update submodule
check_repo(${PNG_DIR}/repo)

# Only aarch64
aarch64_only()

# Generate config.h
set(LT_OBJDIR "./libs")
set(PACKAGE ${PNG_PACKAGE})
set(PACKAGE_BUGREPORT "png-mng-implement@lists.sourceforge.net")
set(PACKAGE_NAME ${PNG_PACKAGE})
set(PACKAGE_STRING "${PNG_PACKAGE} ${PNG_VERSION}")
set(PACKAGE_TARNAME ${PNG_PACKAGE})
set(PACKAGE_URL "https://libpng.sourceforge.io")
set(PACKAGE_VERSION ${PNG_VERSION})
set(PNG_ARM_NEON_API_SUPPORTED 1)
set(PNG_ARM_NEON_CHECK_SUPPORTED 0)
set(PNG_ARM_NEON_OPT 2)
set(VERSION ${PNG_VERSION})
check_symbol_exists("struct tm" "sys/time.h" TM_IN_SYS_TIME)
check_include_file("dlfcn.h" HAVE_DLFCN_H)
check_include_file("inttypes.h" HAVE_INTTYPES_H)
check_include_file("stdint.h" HAVE_STDINT_H)
check_include_file("stdio.h" HAVE_STDIO_H)
check_include_file("stdlib.h" HAVE_STDLIB_H)
check_include_file("strings.h" HAVE_STRINGS_H)
check_include_file("string.h" HAVE_STRING_H)
check_include_file("sys/stat.h" HAVE_SYS_STAT_H)
check_include_file("sys/types.h" HAVE_SYS_TYPES_H)
check_include_file("unistd.h" HAVE_UNISTD_H)
check_function_exists("feenableexcept" HAVE_FEENABLEEXCEPT)
check_function_exists("pow" HAVE_POW)
check_linker_flag(C "-lm" HAVE_LIBM)
check_linker_flag(C "-lz" HAVE_LIBZ)
configure_file(
        ${CONFIG_H_IN}
        ${CONFIG_H_DEST}
        @ONLY
)

# Copy pnglibconf.h
configure_file(
        ${PNGLIBCONF_H_PREBUILT}
        ${PNGLIBCONF_H_DEST}
        COPYONLY
)
file(READ ${PNGLIBCONF_H_DEST} FILE_CONTENT)
string(REPLACE "\n" ";" FILE_LINES "${FILE_CONTENT}")
list(REMOVE_AT FILE_LINES -2)
list(APPEND FILE_LINES "#define PNG_ARM_NEON_API_SUPPORTED")
list(APPEND FILE_LINES "#endif /* PNGLCONF_H */")
file(WRITE ${PNGLIBCONF_H_DEST} "")
foreach(LINE IN LISTS FILE_LINES)
    file(APPEND ${PNGLIBCONF_H_DEST} "${LINE}\n")
endforeach()

# C args
add_compile_options(
        -O2
)

# libpng
add_library(
        png SHARED
        ${PNG_SRC_DIR}/arm/arm_init.c
        ${PNG_SRC_DIR}/arm/filter_neon_intrinsics.c
        ${PNG_SRC_DIR}/arm/palette_neon_intrinsics.c
        ${PNG_SRC_DIR}/png.c
        ${PNG_SRC_DIR}/pngerror.c
        ${PNG_SRC_DIR}/pngget.c
        ${PNG_SRC_DIR}/pngmem.c
        ${PNG_SRC_DIR}/pngpread.c
        ${PNG_SRC_DIR}/pngread.c
        ${PNG_SRC_DIR}/pngrio.c
        ${PNG_SRC_DIR}/pngrtran.c
        ${PNG_SRC_DIR}/pngrutil.c
        ${PNG_SRC_DIR}/pngset.c
        ${PNG_SRC_DIR}/pngtrans.c
        ${PNG_SRC_DIR}/pngwio.c
        ${PNG_SRC_DIR}/pngwrite.c
        ${PNG_SRC_DIR}/pngwtran.c
        ${PNG_SRC_DIR}/pngwutil.c
)

target_link_libraries(png PUBLIC z)

target_include_directories(png PUBLIC ${PNG_SRC_DIR} ${AUTOGEN_DIR})