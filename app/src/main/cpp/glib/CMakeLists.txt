cmake_minimum_required(VERSION 3.22.1)
project(glib)

set(GLIB_VERSION "2.82.1")

set(GLIB_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(GLIB_SRC_DIR ${GLIB_DIR}/repo/glib)
set(GLIB_TOOLS_DIR ${GLIB_DIR}/repo/tools)
set(AUTOGEN_DIR ${GLIB_DIR}/autogen)

set(SUB_INCLUDE_DIR ${AUTOGEN_DIR}/include)
set(GLIB_INCLUDE_DIR ${SUB_INCLUDE_DIR}/glib)

set(CONFIG_H_IN ${GLIB_DIR}/config.h.in)
set(CONFIG_H_DEST ${AUTOGEN_DIR}/config/config.h)

set(GLIBCONFIG_H_IN ${GLIB_DIR}/glibconfig.h.in)
set(GLIBCONFIG_H_DEST ${SUB_INCLUDE_DIR}/glibconfig.h)

set(GVERSIONMACROS_H_IN ${GLIB_SRC_DIR}/gversionmacros.h.in)
set(GVERSIONMACROS_H_DEST ${GLIB_INCLUDE_DIR}/gversionmacros.h)

set(GLIB_VISIBILITY_H_DEST ${GLIB_INCLUDE_DIR}/glib-visibility.h)

# import glib tools
set(PY_GEN_VISIBILITY_MACROS_PATH ${GLIB_TOOLS_DIR}/gen-visibility-macros.py)
include(${GLIB_DIR}/gen-visibility-macros.cmake)

include(CheckFunctionExists)
include(CheckSymbolExists)
include(CheckIncludeFile)
include(CheckStructHasMember)
include(CheckTypeSize)
include(TestBigEndian)

# Check if need to update submodule
check_repo(${GLIB_DIR}/repo)

# Check for versions
string(REPLACE "." ";" VERSION_LIST "${GLIB_VERSION}")
list(GET VERSION_LIST 0 GLIB_MAJOR_VERSION)
list(GET VERSION_LIST 1 GLIB_MINOR_VERSION)
list(GET VERSION_LIST 2 GLIB_MICRO_VERSION)
set(GLIB_INTERFACE_AGE ${GLIB_MICRO_VERSION})
math(EXPR GLIB_BINARY_AGE "100 * ${GLIB_MINOR_VERSION} + ${GLIB_MICRO_VERSION}")
set(GETTEXT_PACKAGE "glib20")
set(PACKAGE_BUGREPORT "https://gitlab.gnome.org/GNOME/glib/issues/new")
set(PACKAGE_NAME "glib")
set(PACKAGE_STRING "glib ${GLIB_VERSION}")
set(PACKAGE_TARNAME "glib")
set(PACKAGE_URL "")
set(PACKAGE_VERSION ${GLIB_VERSION})
set(GLIB_OS "#define G_OS_UNIX")
set(_GNU_SOURCE 1)

# Check for header files
check_include_file("alloca.h" HAVE_ALLOCA_H)
check_include_file("afunix.h" HAVE_AFUNIX_H)
check_include_file("crt_externs.h" HAVE_CRT_EXTERNS_H)
check_include_file("dirent.h" HAVE_DIRENT_H)
check_include_file("float.h" HAVE_FLOAT_H)
check_include_file("fstab.h" HAVE_FSTAB_H)
check_include_file("ftw.h" HAVE_FTW_H)
check_include_file("grp.h" HAVE_GRP_H)
check_include_file("inttypes.h" HAVE_INTTYPES_H)
check_include_file("libproc.h" HAVE_LIBPROC_H)
check_include_file("limits.h" HAVE_LIMITS_H)
check_include_file("locale.h" HAVE_LOCALE_H)
check_include_file("mach/mach_time.h" HAVE_MACH_MACH_TIME_H)
check_include_file("memory.h" HAVE_MEMORY_H)
check_include_file("mntent.h" HAVE_MNTENT_H)
check_include_file("poll.h" HAVE_POLL_H)
check_include_file("pwd.h" HAVE_PWD_H)
check_include_file("sched.h" HAVE_SCHED_H)
check_include_file("spawn.h" HAVE_SPAWN_H)
check_include_file("stdatomic.h" HAVE_STDATOMIC_H)
check_include_file("stdint.h" HAVE_STDINT_H)
check_include_file("stdlib.h" HAVE_STDLIB_H)
check_include_file("string.h" HAVE_STRING_H)
check_include_file("strings.h" HAVE_STRINGS_H)
check_include_file("sys/auxv.h" HAVE_SYS_AUXV_H)
check_include_file("sys/event.h" HAVE_SYS_EVENT_H)
check_include_file("sys/filio.h" HAVE_SYS_FILIO_H)
check_include_file("sys/inotify.h" HAVE_SYS_INOTIFY_H)
check_include_file("sys/mkdev.h" HAVE_SYS_MKDEV_H)
check_include_file("sys/mntctl.h" HAVE_SYS_MNTCTL_H)
check_include_file("sys/mnttab.h" HAVE_SYS_MNTTAB_H)
check_include_file("sys/mount.h" HAVE_SYS_MOUNT_H)
check_include_file("sys/param.h" HAVE_SYS_PARAM_H)
check_include_file("sys/prctl.h" HAVE_SYS_PRCTL_H)
check_include_file("sys/resource.h" HAVE_SYS_RESOURCE_H)
check_include_file("sys/select.h" HAVE_SYS_SELECT_H)
check_include_file("sys/statfs.h" HAVE_SYS_STATFS_H)
check_include_file("sys/stat.h" HAVE_SYS_STAT_H)
check_include_file("sys/statvfs.h" HAVE_SYS_STATVFS_H)
check_include_file("sys/sysctl.h" HAVE_SYS_SYSCTL_H)
check_include_file("sys/time.h" HAVE_SYS_TIME_H)
check_include_file("sys/times.h" HAVE_SYS_TIMES_H)
check_include_file("sys/types.h" HAVE_SYS_TYPES_H)
check_include_file("sys/uio.h" HAVE_SYS_UIO_H)
check_include_file("sys/vfs.h" HAVE_SYS_VFS_H)
check_include_file("sys/vfstab.h" HAVE_SYS_VFSTAB_H)
check_include_file("sys/vmount.h" HAVE_SYS_VMOUNT_H)
check_include_file("sys/wait.h" HAVE_SYS_WAIT_H)
check_include_file("syslog.h" HAVE_SYSLOG_H)
check_include_file("termios.h" HAVE_TERMIOS_H)
check_include_file("unistd.h" HAVE_UNISTD_H)
check_include_file("values.h" HAVE_VALUES_H)
check_include_file("wchar.h" HAVE_WCHAR_H)
check_include_file("xlocale.h" HAVE_XLOCALE_H)
check_include_file("malloc.h" HAVE_MALLOC_H)
check_include_file("linux/netlink.h" HAVE_NETLINK)

# Check for struct members
if (HAVE_LOCALE_H)
    check_symbol_exists(LC_MESSAGES "locale.h" HAVE_LC_MESSAGES)
endif ()
check_struct_has_member(stat st_mtimensec "sys/stat.h" HAVE_STRUCT_STAT_ST_MTIMENSEC)
check_struct_has_member(stat st_mtim.tv_nsec "sys/stat.h" HAVE_STRUCT_STAT_ST_MTIM_TV_NSEC)
check_struct_has_member(stat st_atimensec "sys/stat.h" HAVE_STRUCT_STAT_ST_ATIMENSEC)
check_struct_has_member(stat st_atim.tv_nsec "sys/stat.h" HAVE_STRUCT_STAT_ST_ATIM_TV_NSEC)
check_struct_has_member(stat st_ctimensec "sys/stat.h" HAVE_STRUCT_STAT_ST_CTIMENSEC)
check_struct_has_member(stat st_ctim.tv_nsec "sys/stat.h" HAVE_STRUCT_STAT_ST_CTIM_TV_NSEC)
check_struct_has_member(stat st_birthtime "sys/stat.h" HAVE_STRUCT_STAT_ST_BIRTHTIME)
check_struct_has_member(stat st_birthtimensec "sys/stat.h" HAVE_STRUCT_STAT_ST_BIRTHTIMENSEC)
check_struct_has_member(stat st_birthtim "sys/stat.h" HAVE_STRUCT_STAT_ST_BIRTHTIM)
check_struct_has_member(stat st_birthtim.tv_nsec "sys/stat.h" HAVE_STRUCT_STAT_ST_BIRTHTIM_TV_NSEC)
check_struct_has_member(stat st_blksize "sys/stat.h" HAVE_STRUCT_STAT_ST_BLKSIZE)
check_struct_has_member(stat st_blocks "sys/stat.h" HAVE_STRUCT_STAT_ST_BLOCKS)
check_struct_has_member(statfs f_fstypename "sys/statfs.h" HAVE_STRUCT_STATFS_F_FSTYPENAME)
check_struct_has_member(statfs f_bavail "sys/statfs.h" HAVE_STRUCT_STATFS_F_BAVAIL)
check_struct_has_member(dirent d_type "dirent.h" HAVE_STRUCT_DIRENT_D_TYPE)
check_struct_has_member(statvfs f_basetype "sys/statvfs.h" HAVE_STRUCT_STATVFS_F_BASETYPE)
check_struct_has_member(statvfs f_fstypename "sys/statvfs.h" HAVE_STRUCT_STATVFS_F_FSTYPENAME)
check_struct_has_member(statvfs f_type "sys/statvfs.h" HAVE_STRUCT_STATVFS_F_TYPE)
check_struct_has_member(tm tm_gmtoff "time.h" HAVE_STRUCT_TM_TM_GMTOFF)
check_struct_has_member(tm __tm_gmtoff "time.h" HAVE_STRUCT_TM___TM_GMTOFF)

# Check for functions
check_function_exists(accept4 HAVE_ACCEPT4)
check_function_exists(close_range HAVE_CLOSE_RANGE)
check_function_exists(copy_file_range HAVE_COPY_FILE_RANGE)
check_function_exists(endmntent HAVE_ENDMNTENT)
check_function_exists(endservent HAVE_ENDSERVENT)
check_function_exists(epoll_create1 HAVE_EPOLL_CREATE1)
check_function_exists(fallocate HAVE_FALLOCATE)
check_function_exists(fchmod HAVE_FCHMOD)
check_function_exists(fchown HAVE_FCHOWN)
check_function_exists(fdwalk HAVE_FDWALK)
check_function_exists(free_aligned_sized HAVE_FREE_ALIGNED_SIZED)
check_function_exists(free_sized HAVE_FREE_SIZED)
check_function_exists(fsync HAVE_FSYNC)
check_function_exists(ftruncate64 HAVE_FTRUNCATE64)
check_function_exists(getauxval HAVE_GETAUXVAL)
check_function_exists(getc_unlocked HAVE_GETC_UNLOCKED)
check_function_exists(getfsstat HAVE_GETFSSTAT)
check_function_exists(getgrgid_r HAVE_GETGRGID_R)
check_function_exists(getmntent_r HAVE_GETMNTENT_R)
check_function_exists(getpwuid_r HAVE_GETPWUID_R)
check_function_exists(getresuid HAVE_GETRESUID)
check_function_exists(getvfsstat HAVE_GETVFSSTAT)
check_function_exists(gmtime_r HAVE_GMTIME_R)
check_function_exists(hasmntopt HAVE_HASMNTOPT)
check_function_exists(inotify_init1 HAVE_INOTIFY_INIT1)
check_function_exists(issetugid HAVE_ISSETUGID)
check_function_exists(kevent HAVE_KEVENT)
check_function_exists(kqueue HAVE_KQUEUE)
check_function_exists(lchmod HAVE_LCHMOD)
check_function_exists(lchown HAVE_LCHOWN)
check_function_exists(link HAVE_LINK)
check_function_exists(localtime_r HAVE_LOCALTIME_R)
check_function_exists(lstat HAVE_LSTAT)
check_function_exists(mbrtowc HAVE_MBRTOWC)
check_function_exists(memalign HAVE_MEMALIGN)
check_function_exists(mmap HAVE_MMAP)
check_function_exists(newlocale HAVE_NEWLOCALE)
check_function_exists(pipe2 HAVE_PIPE2)
check_function_exists(poll HAVE_POLL)
check_function_exists(prlimit HAVE_PRLIMIT)
check_function_exists(readlink HAVE_READLINK)
check_function_exists(recvmmsg HAVE_RECVMMSG)
check_function_exists(sendmmsg HAVE_SENDMMSG)
check_function_exists(setenv HAVE_SETENV)
check_function_exists(setmntent HAVE_SETMNTENT)
check_function_exists(strerror_r HAVE_STRERROR_R)
check_function_exists(strnlen HAVE_STRNLEN)
check_function_exists(strsignal HAVE_STRSIGNAL)
check_function_exists(strtod_l HAVE_STRTOD_L)
check_function_exists(strtoll_l HAVE_STRTOLL_L)
check_function_exists(strtoull_l HAVE_STRTOULL_L)
check_function_exists(symlink HAVE_SYMLINK)
check_function_exists(timegm HAVE_TIMEGM)
check_function_exists(unsetenv HAVE_UNSETENV)
check_function_exists(uselocale HAVE_USELOCALE)
check_function_exists(utimes HAVE_UTIMES)
check_function_exists(utimensat HAVE_UTIMENSAT)
check_function_exists(valloc HAVE_VALLOC)
check_function_exists(vasprintf HAVE_VASPRINTF)
check_function_exists(vsnprintf HAVE_VSNPRINTF)
check_function_exists(wcrtomb HAVE_WCRTOMB)
check_function_exists(wcslen HAVE_WCSLEN)
check_function_exists(wcsnlen HAVE_WCSNLEN)
check_function_exists(sysctlbyname HAVE_SYSCTLBYNAME)

if (HAVE_SYS_STATVFS_H)
    check_function_exists(statvfs HAVE_STATVFS)
endif ()

if (HAVE_SYS_STATFS_H OR HAVE_SYS_MOUNT_H)
    check_function_exists(statfs HAVE_STATFS)
endif ()

if (HAVE_SYS_PRCTL_H)
    check_function_exists(prctl HAVE_PRCTL)
endif ()

check_function_exists(if_indextoname HAVE_IF_INDEXTONAME)
check_function_exists(if_nametoindex HAVE_IF_NAMETOINDEX)

check_function_exists(stpcpy HAVE_STPCPY)
check_function_exists(memalign HAVE_MEMALIGN)

check_c_source_compiles("
    #include <netdb.h>
    int main (int argc, char ** argv) {
        int (*fcn)(const char *,
            const char *,
            struct servent *,
            char *,
            size_t,
            struct servent **) = getservbyname_r;
        (void) fcn;
        return 0;
    }
" HAVE_GETSERVBYNAME_R)

check_function_exists(_aligned_malloc HAVE__ALIGNED_MALLOC)
check_function_exists(posix_memalign HAVE_POSIX_MEMALIGN)
check_function_exists(posix_spawn HAVE_POSIX_SPAWN)

# Check whether strerror_r returns char *
check_c_source_compiles("
    #define _GNU_SOURCE
    #include <string.h>
    int main (int argc, char ** argv) {
        char error_string[256];
        char *ptr = strerror_r (-2, error_string, 256);
        char c = *strerror_r (-2, error_string, 256);
        return c != 0 && ptr != (void*) 0L;
    }
" STRERROR_R_CHAR_P)

check_function_exists(snprintf HAVE_SNPRINTF)
check_function_exists(strcasecmp HAVE_STRCASECMP)
check_function_exists(strncasecmp HAVE_STRNCASECMP)

check_symbol_exists(major "sys/sysmacros.h" MAJOR_IN_SYSMACROS)
if (NOT MAJOR_IN_SYSMACROS)
    check_symbol_exists(major "sys/mkdev.h" MAJOR_IN_MKDEV)
    if (NOT MAJOR_IN_MKDEV)
        check_symbol_exists(major "sys/types.h" MAJOR_IN_TYPES)
    endif ()
endif ()

check_symbol_exists(RTLD_LAZY "dlfcn.h" HAVE_RTLD_LAZY)
check_symbol_exists(RTLD_NOW "dlfcn.h" HAVE_RTLD_NOW)
check_symbol_exists(RTLD_GLOBAL "dlfcn.h" HAVE_RTLD_GLOBAL)
check_symbol_exists(RTLD_NEXT "dlfcn.h" HAVE_RTLD_NEXT)
check_symbol_exists(loff_t "sys/types.h" HAVE_LOFF_T)

# Check whether to use statfs or statvfs
# Some systems have both statfs and statvfs, pick the most "native" for these
if (HAVE_SYS_STATVFS_H)
    set(USE_STATVFS 1)
endif ()
if (HAVE_SYS_STATFS_H)
    # at least on linux, statfs is the actual syscall
    unset(USE_STATVFS)
    set(USE_STATFS 1)
endif ()

check_function_exists(mkostemp HAVE_MKOSTEMP)

# Check for futex(2)
check_c_source_compiles("
    #include <linux/futex.h>
    #include <sys/syscall.h>
    #include <unistd.h>
    int main (int argc, char ** argv) {
        syscall (__NR_futex, NULL, FUTEX_WAKE, FUTEX_WAIT);
        return 0;
    }
" HAVE_FUTEX)
check_c_source_compiles("
    #include <linux/futex.h>
    #include <sys/syscall.h>
    #include <unistd.h>
    int main (int argc, char ** argv) {
        syscall (__NR_futex_time64, NULL, FUTEX_WAKE, FUTEX_WAIT);
        return 0;
    }
" HAVE_FUTEX_TIME64)

# Check for eventfd(2)
check_c_source_compiles("
    #include <sys/eventfd.h>
    #include <unistd.h>
    int main (int argc, char ** argv) {
        eventfd (0, EFD_CLOEXEC);
        return 0;
    }
" HAVE_EVENTFD)

# Check for ppoll(2)
check_c_source_compiles("
    #define _GNU_SOURCE
    #include <poll.h>
    #include <stddef.h>
    int main (int argc, char ** argv) {
        struct pollfd fds[1] = {{0}};
        struct timespec ts = {0};
        ppoll (fds, 1, NULL, NULL);
        return 0;
    }
" HAVE_PPOLL)

# Check for pidfd_open(2)
check_c_source_compiles("
    #include <sys/syscall.h>
    #include <sys/wait.h>
    #include <linux/wait.h>
    #include <unistd.h>
    int main (int argc, char ** argv) {
        siginfo_t child_info = { 0, };
        syscall (SYS_pidfd_open, 0, 0);
        waitid (P_PIDFD, 0, &child_info, WEXITED | WNOHANG);
        return 0;
    }
" HAVE_PIDFD)

# Check for __uint128_t (gcc) by checking for 128-bit division
check_c_source_compiles("
    int main(int argc, char ** argv) {
        static __uint128_t v1 = 100;
        static __uint128_t v2 = 10;
        static __uint128_t u;
        u = v1 / v2;
        (void) u;
        return 0;
    }
" HAVE_UINT128_T)

# On Android, clock_gettime is in rt library
check_c_source_compiles("
    #include <time.h>
    struct timespec t;
    int main (int argc, char ** argv) {
        return clock_gettime(CLOCK_REALTIME, &t);
    }
" HAVE_CLOCK_GETTIME)

# On Linux, statfs() has 2 arguments
if (HAVE_STATFS)
    set(STATFS_ARGS 2)
endif ()

# open takes O_DIRECTORY as an option
check_c_source_compiles("
    #include <fcntl.h>
    #include <sys/types.h>
    #include <sys/stat.h>
    int main (int argc, char ** argv) {
        open(\".\", O_DIRECTORY, 0);
        return 0;
    }
" HAVE_OPEN_O_DIRECTORY)

# fcntl takes F_FULLFSYNC as an option
check_c_source_compiles("
    #include <fcntl.h>
    #include <sys/types.h>
    #include <sys/stat.h>
    int main (int argc, char ** argv) {
        fcntl(0, F_FULLFSYNC, 0);
        return 0;
    }
" HAVE_FCNTL_F_FULLFSYNC)

# we won't check if xxx_printf will run when cross building
check_function_exists(vsnprintf HAVE_C99_VSNPRINTF)
check_function_exists(snprintf HAVE_C99_SNPRINTF)
check_function_exists(sprintf HAVE_UNIX98_PRINTF)

set(EXEEXT "")

# Our printf is 'good' only if vsnpintf()/snprintf()/printf() supports C99 well enough
if (HAVE_C99_VSNPRINTF AND HAVE_C99_SNPRINTF AND HAVE_UNIX98_PRINTF)
    set(USE_SYSTEM_PRINTF 1)
    set(GLIB_USING_SYSTEM_PRINTF 1)
endif ()
if (NOT USE_SYSTEM_PRINTF)
    # gnulib has vasprintf so override the previous check
    set(HAVE_VASPRINTF 1)
endif ()

# Check for nl_langinfo and CODESET
check_c_source_compiles("
    #include <langinfo.h>
    int main (int argc, char ** argv) {
        char *codeset = nl_langinfo (CODESET);
        (void) codeset;
        return 0;
    }
" HAVE_LANGINFO_CODESET)
if (HAVE_LANGINFO_CODESET)
    set(HAVE_CODESET 1)
endif ()

# Check for nl_langinfo and LC_TIME parts that are needed in gdatetime.c
check_c_source_compiles("
    #include <langinfo.h>
    int main (int argc, char ** argv) {
        char *str;
        str = nl_langinfo (PM_STR);
        str = nl_langinfo (D_T_FMT);
        str = nl_langinfo (D_FMT);
        str = nl_langinfo (T_FMT);
        str = nl_langinfo (T_FMT_AMPM);
        str = nl_langinfo (MON_1);
        str = nl_langinfo (ABMON_12);
        str = nl_langinfo (DAY_1);
        str = nl_langinfo (ABDAY_7);
        (void) str;
        return 0;
    }
" HAVE_LANGINFO_TIME)

# Linux glibc supports ERA, but FreeBSD and macOS don’t
check_c_source_compiles("
    #include <langinfo.h>
    int main (int argc, char **argv) {
        char *str;
        str = nl_langinfo (ERA);
        str = nl_langinfo (ERA_D_T_FMT);
        str = nl_langinfo (ERA_D_FMT);
        str = nl_langinfo (ERA_T_FMT);
        str = nl_langinfo (_NL_TIME_ERA_NUM_ENTRIES);
        (void) str;
        return 0;
    }
" HAVE_LANGINFO_ERA)
if (HAVE_LANGINFO_ERA AND NOT HAVE_LANGINFO_TIME)
    message(FATAL_ERROR "nl_langinfo(ERA) is supported but more basic nl_langinfo() functionality like PM_STR is not")
endif ()

check_c_source_compiles("
    #include <langinfo.h>
    int main (int argc, char ** argv) {
        char *str;
        str = nl_langinfo (_NL_CTYPE_OUTDIGIT0_MB);
        str = nl_langinfo (_NL_CTYPE_OUTDIGIT1_MB);
        str = nl_langinfo (_NL_CTYPE_OUTDIGIT2_MB);
        str = nl_langinfo (_NL_CTYPE_OUTDIGIT3_MB);
        str = nl_langinfo (_NL_CTYPE_OUTDIGIT4_MB);
        str = nl_langinfo (_NL_CTYPE_OUTDIGIT5_MB);
        str = nl_langinfo (_NL_CTYPE_OUTDIGIT6_MB);
        str = nl_langinfo (_NL_CTYPE_OUTDIGIT7_MB);
        str = nl_langinfo (_NL_CTYPE_OUTDIGIT8_MB);
        str = nl_langinfo (_NL_CTYPE_OUTDIGIT9_MB);
        (void) str;
        return 0;
    }
" HAVE_LANGINFO_OUTDIGIT)
if (HAVE_LANGINFO_OUTDIGIT AND NOT HAVE_LANGINFO_TIME)
    message(FATAL_ERROR "nl_langinfo(_NL_CTYPE_OUTDIGITn_MB) is supported but more basic nl_langinfo() functionality like PM_STR is not")
endif ()

# Check for nl_langinfo and alternative month names
check_c_source_compiles("
    #ifndef _GNU_SOURCE
    # define _GNU_SOURCE
    #endif
    #include <langinfo.h>
    int main (int argc, char ** argv) {
        char *str;
        str = nl_langinfo (ALTMON_1);
        str = nl_langinfo (ALTMON_2);
        str = nl_langinfo (ALTMON_3);
        str = nl_langinfo (ALTMON_4);
        str = nl_langinfo (ALTMON_5);
        str = nl_langinfo (ALTMON_6);
        str = nl_langinfo (ALTMON_7);
        str = nl_langinfo (ALTMON_8);
        str = nl_langinfo (ALTMON_9);
        str = nl_langinfo (ALTMON_10);
        str = nl_langinfo (ALTMON_11);
        str = nl_langinfo (ALTMON_12);
        (void) str;
        return 0;
    }
" HAVE_LANGINFO_ALTMON)
if (HAVE_LANGINFO_ALTMON AND NOT HAVE_LANGINFO_TIME)
    message(FATAL_ERROR "nl_langinfo(ALTMON_n) is supported but more basic nl_langinfo() functionality like PM_STR is not")
endif ()

# Check for nl_langinfo and abbreviated alternative month names
check_c_source_compiles("
    #ifndef _GNU_SOURCE
    # define _GNU_SOURCE
    #endif
    #include <langinfo.h>
    int main (int argc, char ** argv) {
        char *str;
        str = nl_langinfo (_NL_ABALTMON_1);
        str = nl_langinfo (_NL_ABALTMON_2);
        str = nl_langinfo (_NL_ABALTMON_3);
        str = nl_langinfo (_NL_ABALTMON_4);
        str = nl_langinfo (_NL_ABALTMON_5);
        str = nl_langinfo (_NL_ABALTMON_6);
        str = nl_langinfo (_NL_ABALTMON_7);
        str = nl_langinfo (_NL_ABALTMON_8);
        str = nl_langinfo (_NL_ABALTMON_9);
        str = nl_langinfo (_NL_ABALTMON_10);
        str = nl_langinfo (_NL_ABALTMON_11);
        str = nl_langinfo (_NL_ABALTMON_12);
        (void) str;
        return 0;
    }
" HAVE_LANGINFO_ABALTMON)
if (HAVE_LANGINFO_ABALTMON AND NOT HAVE_LANGINFO_TIME)
    message(FATAL_ERROR "nl_langinfo(_NL_ABALTMON_n) is supported but more basic nl_langinfo() functionality like PM_STR is not")
endif ()

# Check for nl_langinfo and _NL_TIME_CODESET
check_c_source_compiles("
    #include <langinfo.h>
    int main (int argc, char ** argv) {
        char *codeset = nl_langinfo (_NL_TIME_CODESET);
        (void) codeset;
        return 0;
    }
" HAVE_LANGINFO_TIME_CODESET)
if (HAVE_LANGINFO_TIME_CODESET AND NOT HAVE_LANGINFO_TIME)
    message(FATAL_ERROR "nl_langinfo(_NL_TIME_CODESET) is supported but more basic nl_langinfo() functionality like PM_STR is not")
endif ()

# Check if C compiler supports the 'signed' keyword
check_c_source_compiles("
    int main (int argc, char ** argv) {
        signed char x;
        return 0;
    }
" SUPPORT_SIGNED)

# Check if the ptrdiff_t type exists
check_symbol_exists(ptrdiff_t "stddef.h" HAVE_PTRDIFF_T)

# Check for sig_atomic_t type
check_c_source_compiles("
    #include <signal.h>
    #include <sys/types.h>
    sig_atomic_t val = 42;
    int main (int argc, char ** argv) {
        return val == 42 ? 0 : 1;
    }
" HAVE_SIG_ATOMIC_T)

# Check if 'long long' works
check_c_source_compiles("
    long long ll = 1LL;
    int i = 63;
    int main (int argc, char **argv) {
        long long llmax = (long long) -1;
        return ll << i | ll >> i | llmax / ll | llmax % ll;
    }
" HAVE_LONG_LONG)

# Test whether the compiler supports the 'long double' type.
check_c_source_compiles("
    int main(int argc, char **argv) {
        /* The Stardent Vistra knows sizeof(long double), but does not support it.  */
        long double foo = 0.0;
        /* On Ultrix 4.3 cc, long double is 4 and double is 8.  */
        int array [2*(sizeof(long double) >= sizeof(double)) - 1];
        return 0;
    }
" HAVE_LONG_DOUBLE)

# Test whether <stddef.h> has the 'wchar_t' type.
check_symbol_exists(wchar_t "stddef.h" HAVE_WCHAR_T)

# Test whether <wchar.h> has the 'wint_t' type.
check_symbol_exists(wint_t "wchar.h" HAVE_WINT_T)

# Define HAVE_INTTYPES_H_WITH_UINTMAX if <inttypes.h> exists,
# doesn't clash with <sys/types.h>, and declares uintmax_t.
check_c_source_compiles("
    #include <sys/types.h>
    #include <inttypes.h>
    int main(int argc, char **argv) {
        uintmax_t i = (uintmax_t) -1;
        (void) i;
        return 0;
    }
" HAVE_INTTYPES_H_WITH_UINTMAX)

# Define HAVE_STDINT_H_WITH_UINTMAX if <stdint.h> exists,
# doesn't clash with <sys/types.h>, and declares uintmax_t.
check_c_source_compiles("
    #include <sys/types.h>
    #include <stdint.h>
    int main(int argc, char **argv) {
        uintmax_t i = (uintmax_t) -1;
        (void) i;
        return 0;
    }
" HAVE_STDINT_H_WITH_UINTMAX)

# Define intmax_t to 'long' or 'long long'
# if it is not already defined in <stdint.h> or <inttypes.h>.
# For simplicity, we assume that a header file defines 'intmax_t' if and
# only if it defines 'uintmax_t'.
if (HAVE_INTTYPES_H_WITH_UINTMAX OR HAVE_STDINT_H_WITH_UINTMAX)
    set(HAVE_INTMAX_T 1)
elseif (HAVE_LONG_LONG)
    set(INTMAX_T "long long")
else ()
    set(INTMAX_T "long")
endif ()

# Check for types size
check_type_size("char" SIZEOF_CHAR)
check_type_size("short" SIZEOF_SHORT)
check_type_size("int" SIZEOF_INT)
check_type_size("void*" SIZEOF_VOID_P)
check_type_size("long" SIZEOF_LONG)
if (HAVE_LONG_LONG)
    check_type_size("long long" SIZEOF_LONG_LONG)
else ()
    set(SIZEOF_LONG_LONG 0)
endif ()
check_type_size("size_t" SIZEOF_SIZE_T)
check_type_size("ssize_t" SIZEOF_SSIZE_T)
check_type_size("wchar_t" SIZEOF_WCHAR_T)

# Generally on aarch64, max alignment size of basic type is equal to the size of it
set(INT64_M "ll")
set(ALIGN_CHAR ${SIZEOF_CHAR})
set(ALIGN_SHORT ${SIZEOF_SHORT})
set(ALIGN_INT ${SIZEOF_INT})
set(ALIGN_VOID_P ${SIZEOF_VOID_P})
set(ALIGN_LONG ${SIZEOF_LONG})
set(ALIGN_LONG_LONG ${SIZEOF_LONG_LONG})
set(ALIGN_SIZE_T ${SIZEOF_SIZE_T})

# Some platforms (Apple) hard-code int64_t to long long instead of
# using long on 64-bit architectures. This can cause type mismatch
# warnings when trying to interface with code using the standard
# library type. Test for the warnings and set gint64 to whichever
# works.
if (SIZEOF_LONG_LONG EQUAL SIZEOF_LONG)
    check_c_source_compiles("
        #pragma GCC diagnostic error \"-Wincompatible-pointer-types\"
        #include <stdint.h>
        #include <stdio.h>
        int main (int argc, char **argv) {
            int64_t i1 = 1;
            long *i2 = &i1;
            (void) i2;
            return 1;
        }
    " INT64_T_IS_LONG)
    if (INT64_T_IS_LONG)
        set(INT64_T_TYPEDEF "long")
    else ()
        check_c_source_compiles("
            #pragma GCC diagnostic error \"-Wincompatible-pointer-types\"
            #include <stdint.h>
            #include <stdio.h>
                int main (int argc, char **argv) {
                int64_t i1 = 1;
                long long *i2 = &i1;
                (void) i2;
                return 1;
            }
        " INT64_T_IS_LONG_LONG)
        if (INT64_T_IS_LONG_LONG)
            set(INT64_T_TYPEDEF "long long")
        else ()
            message(FATAL_ERROR "Cannot detect int64_t typedef")
        endif ()
    endif ()
endif ()

if (SIZEOF_SHORT EQUAL 2)
    set(GINT16 "short")
    set(GINT16_MODIFIER "h")
    set(GINT16_FORMAT "hi")
    set(GUINT16_FORMAT "hu")
elseif (SIZEOF_INT EQUAL 2)
    set(GINT16 "int")
    set(GINT16_MODIFIER "")
    set(GINT16_FORMAT "i")
    set(GUINT16_FORMAT "u")
else ()
    message(FATAL_ERROR "Compiler provides no native 16-bit integer type")
endif ()

if (SIZEOF_SIZE EQUAL 4)
    set(GINT32 "short")
    set(GINT32_MODIFIER "h")
    set(GINT32_FORMAT "hi")
    set(GUINT32_FORMAT "hu")
    set(GUINT32_ALIGN ${ALIGN_SHORT})
elseif (SIZEOF_INT EQUAL 4)
    set(GINT32 "int")
    set(GINT32_MODIFIER "")
    set(GINT32_FORMAT "i")
    set(GUINT32_FORMAT "u")
    set(GUINT32_ALIGN ${ALIGN_INT})
elseif (SIZEOF_LONG EQUAL 4)
    set(GINT32 "long")
    set(GINT32_MODIFIER "l")
    set(GINT32_FORMAT "li")
    set(GUINT32_FORMAT "lu")
    set(GUINT32_ALIGN ${ALIGN_LONG})
else ()
    message(FATAL_ERROR "Compiler provides no native 32-bit integer type")
endif ()

if (SIZEOF_INT EQUAL 8)
    set(GINT64 "int")
    set(GINT64_MODIFIER "")
    set(GINT64_FORMAT "i")
    set(GUINT64_FORMAT "u")
    set(GLIB_EXTENSION "")
    set(GINT64_CONSTANT "(val)")
    set(GUINT64_CONSTANT "(val)")
    set(GUINT64_ALIGN ${ALIGN_INT})
elseif ((SIZEOF_LONG EQUAL 8) AND ((NOT SIZEOF_LONG_LONG EQUAL SIZEOF_LONG) OR (INT64_T_TYPEDEF STREQUAL "long")))
    set(GINT64 "long")
    set(GINT64_MODIFIER "l")
    set(GINT64_FORMAT "li")
    set(GUINT64_FORMAT "lu")
    set(GLIB_EXTENSION "")
    set(GINT64_CONSTANT "(val##L)")
    set(GUINT64_CONSTANT "(val##UL)")
    set(GUINT64_ALIGN ${ALIGN_LONG})
elseif ((SIZEOF_LONG_LONG EQUAL 8) AND ((NOT SIZEOF_LONG_LONG EQUAL SIZEOF_LONG) OR (INT64_T_TYPEDEF STREQUAL "long long")))
    set(GINT64 "long long")
    set(GINT64_MODIFIER "${INT64_M}")
    set(GINT64_FORMAT "${INT64_M}i")
    set(GUINT64_FORMAT "${INT64_M}u")
    set(GLIB_EXTENSION "G_GNUC_EXTENSION ")
    set(GINT64_CONSTANT "(G_GNUC_EXTENSION (val##LL))")
    set(GUINT64_CONSTANT "(G_GNUC_EXTENSION (val##UL))")
    set(GUINT64_ALIGN ${ALIGN_LONG_LONG})
endif ()

set(G_PID_TYPE "int")
set(G_PID_FORMAT "i")
set(G_POLLFD_FORMAT "%d")
set(G_DIR_SEPARATOR "/")
set(G_SEARCHPATH_SEPARATOR ":")

set(OLD_C_FLAGS ${CMAKE_C_FLAGS})
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Werror")
set(TYPE_LIST "short" "int" "long" "long long")
foreach (TYPE_NAME IN LISTS TYPE_LIST)
    string(REGEX REPLACE "[^a-zA-Z0-9]" "_" UPPER_NAME ${TYPE_NAME})
    string(TOUPPER ${UPPER_NAME} UPPER_NAME)
    if (SIZEOF_${UPPER_NAME} EQUAL SIZEOF_SIZE_T)
        # Do separate checks for gcc/clang (and ignore other compilers for now), since
        # we need to explicitly pass -Werror to the compilers.
        # FIXME: https://github.com/mesonbuild/meson/issues/5399

        check_c_source_compiles("
            #include <stddef.h>
            static size_t f (size_t *i) { return *i + 1; }
            int main (int argc, char **argv) {
                unsigned ${TYPE_NAME} i = 0;
             f (&i);
                return 0;
            }
        " G_SIZET_COMPAT_${UPPER_NAME})
    endif ()
endforeach ()
set(CMAKE_C_FLAGS ${OLD_C_FLAGS})

if (G_SIZET_COMPAT_SHORT)
    set(GLIB_SIZE_TYPE_DEFINE "short")
    set(GSIZE_MODIFIER "h")
    set(GSSIZE_MODIFIER "h")
    set(GSIZE_FORMAT "hu")
    set(GSSIZE_FORMAT "hi")
    set(GLIB_MSIZE_TYPE "SHRT")
elseif (G_SIZET_COMPAT_INT)
    set(GLIB_SIZE_TYPE_DEFINE "int")
    set(GSIZE_MODIFIER "")
    set(GSSIZE_MODIFIER "")
    set(GSIZE_FORMAT "u")
    set(GSSIZE_FORMAT "i")
    set(GLIB_MSIZE_TYPE "INT")
elseif (G_SIZET_COMPAT_LONG)
    set(GLIB_SIZE_TYPE_DEFINE "long")
    set(GSIZE_MODIFIER "l")
    set(GSSIZE_MODIFIER "l")
    set(GSIZE_FORMAT "lu")
    set(GSSIZE_FORMAT "li")
    set(GLIB_MSIZE_TYPE "LONG")
elseif (G_SIZET_COMPAT_LONG_LONG)
    set(GLIB_SIZE_TYPE_DEFINE "long long")
    set(GSIZE_MODIFIER ${INT64_M})
    set(GSSIZE_MODIFIER ${INT64_M})
    set(GSIZE_FORMAT "${INT64_M}u")
    set(GSSIZE_FORMAT "${INT64_M}i")
    set(GLIB_MSIZE_TYPE "INT64")
else ()
    message(FATAL_ERROR "Could not determine size of size_t.")
endif ()

if (SIZEOF_VOID_P EQUAL SIZEOF_INT)
    set(GLIB_INTPTR_TYPE_DEFINE "int")
    set(GINTPTR_MODIFIER "")
    set(GINTPTR_FORMAT "i")
    set(GUINTPTR_FORMAT "u")
    set(GLIB_GPI_CAST "(gint)")
    set(GLIB_GPUI_CAST "(guint)")
elseif (SIZEOF_VOID_P EQUAL SIZEOF_LONG)
    set(GLIB_INTPTR_TYPE_DEFINE "long")
    set(GINTPTR_MODIFIER "l")
    set(GINTPTR_FORMAT "ll")
    set(GUINTPTR_FORMAT "lu")
    set(GLIB_GPI_CAST "(glong)")
    set(GLIB_GPUI_CAST "(gulong)")
elseif (SIZEOF_VOID_P EQUAL SIZEOF_LONG_LONG)
    set(GLIB_INTPTR_TYPE_DEFINE "long long")
    set(GINTPTR_MODIFIER ${INT64_M})
    set(GINTPTR_FORMAT "${INT64_M}i")
    set(GUINTPTR_FORMAT "${INT64_M}u")
    set(GLIB_GPI_CAST "(gint64)")
    set(GLIB_GPUI_CAST "(guint64)")
else ()
    message(FATAL_ERROR "Could not determine size of void *")
endif ()

if ((NOT SIZEOF_LONG EQUAL 8) AND (NOT SIZEOF_LONG_LONG EQUAL 8) AND (NOT SIZEOF_INT EQUAL 8))
    message(FATAL_ERROR "GLib requires a 64-bit type. You might want to consider using the GNU C compiler.")
endif ()

math(EXPR GINTBITS "${SIZEOF_INT} * 8")
math(EXPR GLONGBITS "${SIZEOF_LONG} * 8")
math(EXPR GSIZEBITS "${SIZEOF_SIZE_T} * 8")
math(EXPR GSSIZEBITS "${SIZEOF_SSIZE_T} * 8")

set(G_MODULE_SUFFIX "so")

set(GLIB_VOID_P ${SIZEOF_VOID_P})
set(GLIB_LONG ${SIZEOF_LONG})
set(GLIB_SIZE_T ${SIZEOF_SIZE_T})
set(GLIB_SSIZE_T ${SIZEOF_SSIZE_T})
test_big_endian(IS_BIG_ENDIAN)
if (IS_BIG_ENDIAN)
    set(G_BYTE_ORDER "G_BIG_ENDIAN")
    set(G_BS_NATIVE "BE")
    set(G_BS_ALIEN "LE")
else ()
    set(G_BYTE_ORDER "G_LITTLE_ENDIAN")
    set(G_BS_NATIVE "LE")
    set(G_BS_ALIEN "BE")
endif ()

# We check for G_VA_COPY_AS_ARRAY for historical reasons, but we no longer
# use it: use Standard C va_copy() instead.
# We won't run any test when cross building
set(GLIB_VACOPY "\n#define G_VA_COPY_AS_ARRAY 1")

# check for flavours of varargs macros
check_c_source_compiles("
    #define call_a(...) a(1,__VA_ARGS__)
    int a(int p1, int p2, int p3) {
        return 0;
    }
    int main (int argc, char **argv) {
        call_a(2,3);
        return 0;
    }
" G_HAVE_ISO_C_VARARGS)
if (NOT G_HAVE_ISO_C_VARARGS)
    message(FATAL_ERROR "GLib requires a C compiler with support for C99 __VA_ARG__ in macros.")
endif ()

check_include_file("alloca.h" GLIB_HAVE_ALLOCA_H)
check_include_file("sys/poll.h" GLIB_HAVE_SYS_POLL_H)

# FIXME: there's not a function like AC_COMPUTE_INT in cmake
# used some pre-defined values as a fallback, from poll.h in sysroot of ndk
set(G_POLLIN 1)
set(G_POLLOUT 4)
set(G_POLLPRI 2)
set(G_POLLERR 8)
set(G_POLLHUP 16)
set(G_POLLNVAL 32)

# used some pre-defined values as a fallback, from sys/socket.h in sysroot of ndk
set(G_AF_UNIX 1)
set(G_AF_INET 2)
set(G_AF_INET6 10)
set(G_MSG_OOB 1)
set(G_MSG_PEEK 2)
set(G_MSG_DONTROUTE 4)

check_symbol_exists("struct in6_addr" "netinet/in.h" HAVE_IPV6)

check_c_source_compiles("
    int main(int argc, char **argv) {
        int atomic = 2;
        __sync_bool_compare_and_swap (&atomic, 2, 3);
        return 0;
    }
" ATOMICTEST)
if (ATOMICTEST)
    check_c_source_compiles("
        #ifndef __GCC_HAVE_SYNC_COMPARE_AND_SWAP_4
        #error \"compiler has atomic ops, but doesn't define __GCC_HAVE_SYNC_COMPARE_AND_SWAP_4\"
        #endif
    " __GCC_HAVE_SYNC_COMPARE_AND_SWAP_4)
    check_c_source_compiles("
        int main(int argc, char **argv) {
            int atomic = 2;
            __sync_swap (&atomic, 2);
            return 0;
        }
    " _GLIB_GCC_HAVE_SYNC_SWAP)
    set(G_ATOMIC_LOCK_FREE 1)
endif ()

# Determination of thread implementation
set(G_THREADS_IMPL_DEF "POSIX")
set(THREADS_POSIX 1)
check_symbol_exists(pthread_attr_setstacksize "pthread.h" HAVE_PTHREAD_ATTR_SETSTACKSIZE)
check_symbol_exists(pthread_attr_setinheritsched "pthread.h" HAVE_PTHREAD_ATTR_SETINHERITSCHED)
check_symbol_exists(pthread_condattr_setclock "pthread.h" HAVE_PTHREAD_CONDATTR_SETCLOCK)
check_symbol_exists(pthread_cond_timedwait_relative_np "pthread.h" HAVE_PTHREAD_COND_TIMEDWAIT_RELATIVE_NP)
check_symbol_exists(pthread_getname_np "pthread.h" HAVE_PTHREAD_GETNAME_NP)
check_symbol_exists(pthread_getaffinity_np "pthread.h" HAVE_PTHREAD_GETAFFINITY_NP)

# Assume that pthread_setname_np is available in some form; same as configure
# Linux, Solaris, etc.
check_c_source_compiles("
    #ifndef _GNU_SOURCE
    # define _GNU_SOURCE
    #endif
    #include <pthread.h>
    int main(int argc, char **argv) {
        pthread_setname_np(pthread_self(), \"example\");
        return 0;
    }
" HAVE_PTHREAD_SETNAME_NP_WITH_TID)

check_function_exists(bind_textdomain_codeset HAVE_BIND_TEXTDOMAIN_CODESET)

# We require gettext to always be present
set(HAVE_DCGETTEXT 1)
set(HAVE_GETTEXT 1)
set(GLIB_LOCALE_DIR "")
set(GLIB_LOCALSTATEDIR "")
set(GLIB_RUNSTATEDIR "")
set(GLIB_CHARSETALIAS_DIR "")

# Check for xattr
check_function_exists(getxattr HAVE_GETXATTR)
check_include_file("sys/xattr.h" HAVE_SYS_XATTR_H)
if (HAVE_GETXATTR AND HAVE_SYS_XATTR_H)
    set(HAVE_SYS_XATTR_H 1)
else ()
    unset(HAVE_SYS_XATTR_H)
endif ()
if (NOT HAVE_SYS_XATTR_H)
    check_symbol_exists(getxattr "attr/xattr.h" HAVE_ATTR_XATTR_H)
endif ()
if (NOT HAVE_SYS_XATTR_H AND NOT HAVE_ATTR_XATTR_H)
    message(FATAL_ERROR "No getxattr implementation found in C library or libxattr")
endif ()
set(HAVE_XATTR 1)

if (HAVE_SYS_TYPES_H)
    set(TMP_HAVE_SYS_TYPES_H 1)
else ()
    set(TMP_HAVE_SYS_TYPES_H 0)
endif ()
if (HAVE_SYS_XATTR_H)
    set(TMP_HAVE_SYS_XATTR_H 1)
else ()
    set(TMP_HAVE_SYS_XATTR_H 0)
endif ()
if (HAVE_ATTR_XATTR_H)
    set(TMP_HAVE_ATTR_XATTR_H 1)
else ()
    set(TMP_HAVE_ATTR_XATTR_H 0)
endif ()
check_c_source_compiles("
    #include <stdio.h>
    #if ${TMP_HAVE_SYS_TYPES_H}
    #include <sys/types.h>
    #endif
    #if ${TMP_HAVE_SYS_XATTR_H}
    #include <sys/xattr.h>
    #elif ${TMP_HAVE_ATTR_XATTR_H}
    #include <attr/xattr.h>
    #endif

    int main (int argc, char **argv) {
        ssize_t len = getxattr(\"\", \"\", NULL, 0, 0, XATTR_NOFOLLOW);
        return len;
    }
"  HAVE_XATTR_NOFOLLOW)

check_function_exists(strlcpy HAVE_STRLCPY)

file(MAKE_DIRECTORY ${AUTOGEN_DIR})
file(MAKE_DIRECTORY ${AUTOGEN_DIR}/config)
file(MAKE_DIRECTORY ${GLIB_INCLUDE_DIR})
file(MAKE_DIRECTORY ${GLIB_INCLUDE_DIR}/deprecated)

# Generate glibconfig.h
configure_file(
        ${GLIBCONFIG_H_IN}
        ${GLIBCONFIG_H_DEST}
        @ONLY
)

# Generate gversionmacros.h
gen_visibility_macros(
        ${PY_GEN_VISIBILITY_MACROS_PATH}
        ${GLIB_VERSION}
        "versions-macros"
        ${GVERSIONMACROS_H_IN}
        ${GVERSIONMACROS_H_DEST}
)

# Generate glib-visibility.h
gen_visibility_macros(
        ${PY_GEN_VISIBILITY_MACROS_PATH}
        ${GLIB_VERSION}
        "visibility-macros"
        "GLIB"
        ${GLIB_VISIBILITY_H_DEST}
)

# NDK doesn't have libintl.h, so we generate a fake one here
file(TOUCH ${AUTOGEN_DIR}/config/libintl.h)

# FIXME: To do what when not using system printf here?
if (USE_SYSTEM_PRINTF)
    set(GL_UNUSED "")
    set(GL_EXTERN_INLINE "")
endif ()

# Install glib headers
set(GLIB_HEADERS
        "glib.h"
        "glib-object.h"
        "glib-autocleanups.h"
        "glib-typeof.h"
        "galloca.h"
        "garray.h"
        "gasyncqueue.h"
        "gatomic.h"
        "gbacktrace.h"
        "gbase64.h"
        "gbitlock.h"
        "gbookmarkfile.h"
        "gbytes.h"
        "gcharset.h"
        "gchecksum.h"
        "gconvert.h"
        "gdataset.h"
        "gdate.h"
        "gdatetime.h"
        "gdir.h"
        "genviron.h"
        "gerror.h"
        "gfileutils.h"
        "ggettext.h"
        "ghash.h"
        "ghmac.h"
        "ghook.h"
        "ghostutils.h"
        "gi18n.h"
        "gi18n-lib.h"
        "giochannel.h"
        "gkeyfile.h"
        "glist.h"
        "gmacros.h"
        "gmain.h"
        "gmappedfile.h"
        "gmarkup.h"
        "gmem.h"
        "gmessages.h"
        "gnode.h"
        "goption.h"
        "gpathbuf.h"
        "gpattern.h"
        "gpoll.h"
        "gprimes.h"
        "gqsort.h"
        "gquark.h"
        "gqueue.h"
        "grand.h"
        "grcbox.h"
        "grefcount.h"
        "grefstring.h"
        "gregex.h"
        "gscanner.h"
        "gsequence.h"
        "gshell.h"
        "gslice.h"
        "gslist.h"
        "gspawn.h"
        "gstdio.h"
        "gstrfuncs.h"
        "gstrvbuilder.h"
        "gtestutils.h"
        "gstring.h"
        "gstringchunk.h"
        "gthread.h"
        "gthreadpool.h"
        "gtimer.h"
        "gtimezone.h"
        "gtrashstack.h"
        "gtree.h"
        "gtypes.h"
        "guuid.h"
        "gunicode.h"
        "guri.h"
        "gutils.h"
        "gvarianttype.h"
        "gvariant.h"
        "gversion.h"
        "gprintf.h"
)
foreach (INCLUDE_FILE IN LISTS GLIB_HEADERS)
    configure_file("${GLIB_SRC_DIR}/${INCLUDE_FILE}" ${GLIB_INCLUDE_DIR} COPYONLY)
endforeach ()

# Install glib deprecated headers
set(GLIB_DEPRECATED_HEADERS
        "deprecated/gallocator.h"
        "deprecated/gcache.h"
        "deprecated/gcompletion.h"
        "deprecated/gmain.h"
        "deprecated/grel.h"
        "deprecated/gthread.h"
)
foreach (INCLUDE_FILE IN LISTS GLIB_DEPRECATED_HEADERS)
    configure_file("${GLIB_SRC_DIR}/${INCLUDE_FILE}" ${GLIB_INCLUDE_DIR}/deprecated COPYONLY)
endforeach ()

# Generate config.h
configure_file(
        ${CONFIG_H_IN}
        ${CONFIG_H_DEST}
        @ONLY
)

# C args
add_compile_options(
        -O2
        -Wduplicated-branches
        -Wimplicit-fallthrough
        -Wmisleading-indentation
        -Wmissing-field-initializers
        -Wnonnull
        -Wnull-dereference
        -Wunused
        -Wno-unused-parameter
        -Wno-cast-function-type
        -Wno-pedantic
        -Wno-format-zero-length
        -Wno-variadic-macros
        -Werror=format=2
        -Werror=init-self
        -Werror=missing-include-dirs
        -Werror=pointer-arith
        -Werror=unused-result
        -Wstrict-prototypes
        -Wno-bad-function-cast
        -Werror=implicit-function-declaration
        -Werror=missing-prototypes
        -Werror=pointer-sign
        -Wno-string-plus-int
        -Wno-typedef-redefinition
        -Wno-unused-value
)

add_link_options(
        -Wl,-z,nodelete
)

# libcharset
add_library(
        charset STATIC
        ${GLIB_SRC_DIR}/libcharset/localcharset.c
)

target_compile_definitions(
        charset PRIVATE
        -DGLIB_CHARSETALIAS_DIR="${GLIB_CHARSETALIAS_DIR}"
)

target_include_directories(
        charset PRIVATE
        ${AUTOGEN_DIR}/config
)

# libglib-2.0
add_library(
        glib-2.0 SHARED
        # glib/deprecated
        ${GLIB_SRC_DIR}/deprecated/gallocator.c
        ${GLIB_SRC_DIR}/deprecated/gcache.c
        ${GLIB_SRC_DIR}/deprecated/gcompletion.c
        ${GLIB_SRC_DIR}/deprecated/grel.c
        ${GLIB_SRC_DIR}/deprecated/gthread-deprecated.c
        # glib
        ${GLIB_SRC_DIR}/garcbox.c
        ${GLIB_SRC_DIR}/garray.c
        ${GLIB_SRC_DIR}/gasyncqueue.c
        ${GLIB_SRC_DIR}/gatomic.c
        ${GLIB_SRC_DIR}/gbacktrace.c
        ${GLIB_SRC_DIR}/gbase64.c
        ${GLIB_SRC_DIR}/gbitlock.c
        ${GLIB_SRC_DIR}/gbookmarkfile.c
        ${GLIB_SRC_DIR}/gbytes.c
        ${GLIB_SRC_DIR}/gcharset.c
        ${GLIB_SRC_DIR}/gchecksum.c
        ${GLIB_SRC_DIR}/gconvert.c
        ${GLIB_SRC_DIR}/gdataset.c
        ${GLIB_SRC_DIR}/gdate.c
        ${GLIB_SRC_DIR}/gdatetime.c
        ${GLIB_SRC_DIR}/gdatetime-private.c
        ${GLIB_SRC_DIR}/gdir.c
        ${GLIB_SRC_DIR}/genviron.c
        ${GLIB_SRC_DIR}/gerror.c
        ${GLIB_SRC_DIR}/gfileutils.c
        ${GLIB_SRC_DIR}/ggettext.c
        ${GLIB_SRC_DIR}/ghash.c
        ${GLIB_SRC_DIR}/ghmac.c
        ${GLIB_SRC_DIR}/ghook.c
        ${GLIB_SRC_DIR}/ghostutils.c
        ${GLIB_SRC_DIR}/giochannel.c
        ${GLIB_SRC_DIR}/gkeyfile.c
        ${GLIB_SRC_DIR}/glib-init.c
        ${GLIB_SRC_DIR}/glib-private.c
        ${GLIB_SRC_DIR}/glist.c
        ${GLIB_SRC_DIR}/gmain.c
        ${GLIB_SRC_DIR}/gmappedfile.c
        ${GLIB_SRC_DIR}/gmarkup.c
        ${GLIB_SRC_DIR}/gmem.c
        ${GLIB_SRC_DIR}/gmessages.c
        ${GLIB_SRC_DIR}/gnode.c
        ${GLIB_SRC_DIR}/goption.c
        ${GLIB_SRC_DIR}/gpathbuf.c
        ${GLIB_SRC_DIR}/gpattern.c
        ${GLIB_SRC_DIR}/gpoll.c
        ${GLIB_SRC_DIR}/gprimes.c
        ${GLIB_SRC_DIR}/gqsort.c
        ${GLIB_SRC_DIR}/gquark.c
        ${GLIB_SRC_DIR}/gqueue.c
        ${GLIB_SRC_DIR}/grand.c
        ${GLIB_SRC_DIR}/grcbox.c
        ${GLIB_SRC_DIR}/grefcount.c
        ${GLIB_SRC_DIR}/grefstring.c
        ${GLIB_SRC_DIR}/gregex.c
        ${GLIB_SRC_DIR}/gscanner.c
        ${GLIB_SRC_DIR}/gsequence.c
        ${GLIB_SRC_DIR}/gshell.c
        ${GLIB_SRC_DIR}/gslice.c
        ${GLIB_SRC_DIR}/gslist.c
        ${GLIB_SRC_DIR}/gspawn.c
        ${GLIB_SRC_DIR}/gstdio.c
        ${GLIB_SRC_DIR}/gstrfuncs.c
        ${GLIB_SRC_DIR}/gstring.c
        ${GLIB_SRC_DIR}/gstringchunk.c
        ${GLIB_SRC_DIR}/gstrvbuilder.c
        ${GLIB_SRC_DIR}/gtestutils.c
        ${GLIB_SRC_DIR}/gthread.c
        ${GLIB_SRC_DIR}/gthreadpool.c
        ${GLIB_SRC_DIR}/gtimer.c
        ${GLIB_SRC_DIR}/gtimezone.c
        ${GLIB_SRC_DIR}/gtrace.c
        ${GLIB_SRC_DIR}/gtranslit.c
        ${GLIB_SRC_DIR}/gtrashstack.c
        ${GLIB_SRC_DIR}/gtree.c
        ${GLIB_SRC_DIR}/guniprop.c
        ${GLIB_SRC_DIR}/gutf8.c
        ${GLIB_SRC_DIR}/gunibreak.c
        ${GLIB_SRC_DIR}/gunicollate.c
        ${GLIB_SRC_DIR}/gunidecomp.c
        ${GLIB_SRC_DIR}/guri.c
        ${GLIB_SRC_DIR}/gutils.c
        ${GLIB_SRC_DIR}/guuid.c
        ${GLIB_SRC_DIR}/gvariant.c
        ${GLIB_SRC_DIR}/gvariant-core.c
        ${GLIB_SRC_DIR}/gvariant-parser.c
        ${GLIB_SRC_DIR}/gvariant-serialiser.c
        ${GLIB_SRC_DIR}/gvarianttypeinfo.c
        ${GLIB_SRC_DIR}/gvarianttype.c
        ${GLIB_SRC_DIR}/gversion.c
        ${GLIB_SRC_DIR}/gwakeup.c
        ${GLIB_SRC_DIR}/gprintf.c
        # unix
        ${GLIB_SRC_DIR}/glib-unix.c
        ${GLIB_SRC_DIR}/gspawn-posix.c
        ${GLIB_SRC_DIR}/giounix.c
)

target_compile_definitions(
        glib-2.0 PRIVATE
        -DGLIB_COMPILATION
)

target_link_libraries(
        glib-2.0 PUBLIC
        charset
        pcre2-8
)

target_include_directories(
        glib-2.0 PUBLIC
        ${SUB_INCLUDE_DIR}
        ${SUB_INCLUDE_DIR}/glib
)

target_include_directories(
        glib-2.0 PRIVATE
        ${AUTOGEN_DIR}/config
        # It is sad that we have to add both ${GLIB_DIR}/repo and ${GLIB_SRC_DIR} here,
        # since some codes use #include with 'glib/' as prefix and some not.
        ${GLIB_DIR}/repo
        ${GLIB_SRC_DIR}
)