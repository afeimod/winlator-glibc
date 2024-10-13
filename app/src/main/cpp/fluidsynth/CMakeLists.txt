cmake_minimum_required(VERSION 3.22.1)
project(fluidsynth)

set(FLUIDSYNTH_PACKAGE "libfluidsynth")
set(FLUIDSYNTH_VERSION "2.3.6")

set(FLUIDSYNTH_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(FLUIDSYNTH_SRC_DIR ${FLUIDSYNTH_DIR}/repo/src)
set(FLUIDSYNTH_INCLUDE_DIR ${FLUIDSYNTH_DIR}/repo/include)
set(AUTOGEN_DIR ${FLUIDSYNTH_DIR}/autogen)

set(CONFIG_H_IN ${FLUIDSYNTH_SRC_DIR}/config.cmake)
set(CONFIG_H_DEST ${AUTOGEN_DIR}/config/config.h)

set(FLUIDSYNTH_H_IN ${FLUIDSYNTH_INCLUDE_DIR}/fluidsynth.cmake)
set(FLUIDSYNTH_H_DEST ${AUTOGEN_DIR}/include/fluidsynth.h)

set(VERSION_H_IN ${FLUIDSYNTH_INCLUDE_DIR}/fluidsynth/version.h.in)
set(VERSION_H_DEST ${AUTOGEN_DIR}/include/fluidsynth/version.h)

set(FLUIDSYNTH_BIN_DIR ${FLUIDSYNTH_DIR}/bin)
include(${FLUIDSYNTH_DIR}/make_tables.cmake)

include(CheckCCompilerFlag)
include(CheckIncludeFile)
include(CheckSymbolExists)
include(CheckTypeSize)
include(TestBigEndian)
test_big_endian(WORDS_BIGENDIAN)

# Check if need to update submodule
check_repo(${FLUIDSYNTH_DIR}/repo)

# the default C standard to use for all targets
set(CMAKE_C_STANDARD 90)

# the default C++ standard to use for all targets
set(CMAKE_CXX_STANDARD 98)

# whether to use gnu extensions
set(CMAKE_C_EXTENSIONS ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Check C
check_include_file(string.h HAVE_STRING_H)
check_include_file(strings.h HAVE_STRINGS_H)
check_include_file(stdlib.h HAVE_STDLIB_H)
check_include_file(stdio.h HAVE_STDIO_H)
check_include_file(math.h HAVE_MATH_H)
check_include_file(errno.h HAVE_ERRNO_H)
check_include_file(stdarg.h HAVE_STDARG_H)
check_include_file(unistd.h HAVE_UNISTD_H)
check_include_file(sys/mman.h HAVE_SYS_MMAN_H)
check_include_file(sys/types.h HAVE_SYS_TYPES_H)
check_include_file(sys/time.h HAVE_SYS_TIME_H)
check_include_file(sys/stat.h HAVE_SYS_STAT_H)
check_include_file(fcntl.h HAVE_FCNTL_H)
check_include_file(sys/socket.h HAVE_SYS_SOCKET_H)
check_include_file(netinet/in.h HAVE_NETINET_IN_H)
check_include_file(netinet/tcp.h HAVE_NETINET_TCP_H)
check_include_file(arpa/inet.h HAVE_ARPA_INET_H)
check_include_file(limits.h  HAVE_LIMITS_H)
check_include_file(pthread.h HAVE_PTHREAD_H)
check_include_file(signal.h HAVE_SIGNAL_H)
check_include_file(getopt.h HAVE_GETOPT_H)
check_include_file(stdint.h HAVE_STDINT_H)
check_type_size("long long" LONG_LONG)

# define some warning flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -W -Wpointer-arith -Wcast-qual -Wstrict-prototypes -Wno-unused-parameter -Wdeclaration-after-statement -Werror=implicit-function-declaration")
check_c_compiler_flag("-Werror=incompatible-pointer-types" HAVE_INCOMPATIBLE_POINTER_TYPES)
if (HAVE_INCOMPATIBLE_POINTER_TYPES)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Werror=incompatible-pointer-types")
endif (HAVE_INCOMPATIBLE_POINTER_TYPES)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -W -Wpointer-arith -Wcast-qual -Wno-unused-parameter")

# gcc and clang support bad function cast and alignment warnings; add them as well.
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wbad-function-cast -Wcast-align")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wcast-align")

# Check for threads and math
find_library(HAS_LIBM NAMES "m")
if (HAS_LIBM)
    set(MATH_LIBRARY "m")
endif (HAS_LIBM)

# Compile with OpenSLES support, Android need it
set(OPENSLES_SUPPORT 1)

# Enable multi-threading support (such as parallel voice synthesis)
set(ENABLE_MIXER_THREADS 1)

# Enable OpenMP support (parallelization of soundfont decoding, vectorization of voice mixing, etc.)
set(HAVE_OPENMP 1)

# Check for C99 float math
CHECK_SYMBOL_EXISTS( sinf "math.h" HAVE_SINF)
CHECK_SYMBOL_EXISTS( cosf "math.h" HAVE_COSF)
CHECK_SYMBOL_EXISTS( fabsf "math.h" HAVE_FABSF)
CHECK_SYMBOL_EXISTS( powf "math.h" HAVE_POWF)
CHECK_SYMBOL_EXISTS( sqrtf "math.h" HAVE_SQRTF)
CHECK_SYMBOL_EXISTS( logf "math.h" HAVE_LOGF)
check_type_size(socklen_t SOCKLEN_T)
if (HAVE_SOCKLEN_T)
    set(HAVE_SOCKLEN_T 1)
endif (HAVE_SOCKLEN_T)

file(MAKE_DIRECTORY ${AUTOGEN_DIR})
file(MAKE_DIRECTORY ${AUTOGEN_DIR}/config)
file(MAKE_DIRECTORY ${AUTOGEN_DIR}/include)
file(MAKE_DIRECTORY ${AUTOGEN_DIR}/include/fluidsynth)

# Generate config.h
configure_file(
        ${CONFIG_H_IN}
        ${CONFIG_H_DEST}
        @ONLY
)

# Generate fluidsynth.h
configure_file(
        ${FLUIDSYNTH_H_IN}
        ${FLUIDSYNTH_H_DEST}
        @ONLY
)

# Generate version.h
string(REPLACE "." ";" VERSION_LIST "${FLUIDSYNTH_VERSION}")
list(GET VERSION_LIST 0 FLUIDSYNTH_VERSION_MAJOR)
list(GET VERSION_LIST 1 FLUIDSYNTH_VERSION_MINOR)
list(GET VERSION_LIST 2 FLUIDSYNTH_VERSION_MICRO)
set(FLUIDSYNTH_VERSION "\"${FLUIDSYNTH_VERSION}\"")
configure_file(
        ${VERSION_H_IN}
        ${VERSION_H_DEST}
        @ONLY
)

# Install public headers
set(FLUIDSYNTH_HEADERS
        "${FLUIDSYNTH_INCLUDE_DIR}/fluidsynth/audio.h"
        "${FLUIDSYNTH_INCLUDE_DIR}/fluidsynth/event.h"
        "${FLUIDSYNTH_INCLUDE_DIR}/fluidsynth/gen.h"
        "${FLUIDSYNTH_INCLUDE_DIR}/fluidsynth/ladspa.h"
        "${FLUIDSYNTH_INCLUDE_DIR}/fluidsynth/log.h"
        "${FLUIDSYNTH_INCLUDE_DIR}/fluidsynth/midi.h"
        "${FLUIDSYNTH_INCLUDE_DIR}/fluidsynth/misc.h"
        "${FLUIDSYNTH_INCLUDE_DIR}/fluidsynth/mod.h"
        "${FLUIDSYNTH_INCLUDE_DIR}/fluidsynth/seq.h"
        "${FLUIDSYNTH_INCLUDE_DIR}/fluidsynth/seqbind.h"
        "${FLUIDSYNTH_INCLUDE_DIR}/fluidsynth/settings.h"
        "${FLUIDSYNTH_INCLUDE_DIR}/fluidsynth/sfont.h"
        "${FLUIDSYNTH_INCLUDE_DIR}/fluidsynth/shell.h"
        "${FLUIDSYNTH_INCLUDE_DIR}/fluidsynth/synth.h"
        "${FLUIDSYNTH_INCLUDE_DIR}/fluidsynth/types.h"
        "${FLUIDSYNTH_INCLUDE_DIR}/fluidsynth/voice.h"
)
foreach (HEADER_FILE IN LISTS FLUIDSYNTH_HEADERS)
    configure_file(
            ${HEADER_FILE}
            ${AUTOGEN_DIR}/include/fluidsynth
            COPYONLY
    )
endforeach ()

# Generate .inc.h files
make_tables(${AUTOGEN_DIR}/config/)

add_compile_options(
        -Wno-strict-prototypes
        -Wno-unused-but-set-variable
)

# libfluidsynth
add_library(
        fluidsynth SHARED
        # src/utils
        ${FLUIDSYNTH_SRC_DIR}/utils/fluid_conv.c
        ${FLUIDSYNTH_SRC_DIR}/utils/fluid_hash.c
        ${FLUIDSYNTH_SRC_DIR}/utils/fluid_list.c
        ${FLUIDSYNTH_SRC_DIR}/utils/fluid_ringbuffer.c
        ${FLUIDSYNTH_SRC_DIR}/utils/fluid_settings.c
        ${FLUIDSYNTH_SRC_DIR}/utils/fluid_sys.c
        # src/sfloader
        ${FLUIDSYNTH_SRC_DIR}/sfloader/fluid_defsfont.c
        ${FLUIDSYNTH_SRC_DIR}/sfloader/fluid_sfont.c
        ${FLUIDSYNTH_SRC_DIR}/sfloader/fluid_sffile.c
        ${FLUIDSYNTH_SRC_DIR}/sfloader/fluid_samplecache.c
        # src/rvoice
        ${FLUIDSYNTH_SRC_DIR}/rvoice/fluid_adsr_env.c
        ${FLUIDSYNTH_SRC_DIR}/rvoice/fluid_chorus.c
        ${FLUIDSYNTH_SRC_DIR}/rvoice/fluid_iir_filter.c
        ${FLUIDSYNTH_SRC_DIR}/rvoice/fluid_lfo.c
        ${FLUIDSYNTH_SRC_DIR}/rvoice/fluid_rvoice.c
        ${FLUIDSYNTH_SRC_DIR}/rvoice/fluid_rvoice_dsp.c
        ${FLUIDSYNTH_SRC_DIR}/rvoice/fluid_rvoice_event.c
        ${FLUIDSYNTH_SRC_DIR}/rvoice/fluid_rvoice_mixer.c
        ${FLUIDSYNTH_SRC_DIR}/rvoice/fluid_rev.c
        # src/synth
        ${FLUIDSYNTH_SRC_DIR}/synth/fluid_chan.c
        ${FLUIDSYNTH_SRC_DIR}/synth/fluid_event.c
        ${FLUIDSYNTH_SRC_DIR}/synth/fluid_gen.c
        ${FLUIDSYNTH_SRC_DIR}/synth/fluid_mod.c
        ${FLUIDSYNTH_SRC_DIR}/synth/fluid_synth.c
        ${FLUIDSYNTH_SRC_DIR}/synth/fluid_synth_monopoly.c
        ${FLUIDSYNTH_SRC_DIR}/synth/fluid_tuning.c
        ${FLUIDSYNTH_SRC_DIR}/synth/fluid_voice.c
        # src/midi
        ${FLUIDSYNTH_SRC_DIR}/midi/fluid_midi.c
        ${FLUIDSYNTH_SRC_DIR}/midi/fluid_midi_router.c
        ${FLUIDSYNTH_SRC_DIR}/midi/fluid_seqbind.c
        ${FLUIDSYNTH_SRC_DIR}/midi/fluid_seqbind_notes.cpp
        ${FLUIDSYNTH_SRC_DIR}/midi/fluid_seq.c
        ${FLUIDSYNTH_SRC_DIR}/midi/fluid_seq_queue.cpp
        # src/drivers
        ${FLUIDSYNTH_SRC_DIR}/drivers/fluid_adriver.c
        ${FLUIDSYNTH_SRC_DIR}/drivers/fluid_mdriver.c
        # src/bindings
        ${FLUIDSYNTH_SRC_DIR}/bindings/fluid_cmd.c
        ${FLUIDSYNTH_SRC_DIR}/bindings/fluid_filerenderer.c
        ${FLUIDSYNTH_SRC_DIR}/bindings/fluid_ladspa.c
        # opensles
        ${FLUIDSYNTH_SRC_DIR}/drivers/fluid_opensles.c
)

target_include_directories(
        fluidsynth PUBLIC
        ${AUTOGEN_DIR}/include
)

target_include_directories(
        fluidsynth PRIVATE
        ${AUTOGEN_DIR}/config
        ${FLUIDSYNTH_SRC_DIR}
        ${FLUIDSYNTH_SRC_DIR}/bindings
        ${FLUIDSYNTH_SRC_DIR}/drivers
        ${FLUIDSYNTH_SRC_DIR}/midi
        ${FLUIDSYNTH_SRC_DIR}/rvoice
        ${FLUIDSYNTH_SRC_DIR}/sfloader
        ${FLUIDSYNTH_SRC_DIR}/synth
        ${FLUIDSYNTH_SRC_DIR}/utils
)

target_link_libraries(
        fluidsynth PUBLIC
        glib-2.0
        OpenSLES
)