#ifndef	_COMM_H_
#define	_COMM_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>
#include <time.h>
#include <inttypes.h>
#include <assert.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <utmp.h>
#include <termios.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <sys/shm.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <sys/syscall.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/timeb.h>

#include <string>
#include <cstdio>
#include <list>
#include <vector>
#include <algorithm>
#include <sstream>
#include <iostream>
#include <cstdlib>
#include <getopt.h>
#include <iomanip>
#include <fstream>
#include <iterator>

#include <linux/limits.h>

#include "build_info.h"

typedef uint32_t u32;
typedef int64_t s64;

#define MIN(a,b)  (((a) < (b)) ? (a) : (b))
#define MAX(a,b)  (((a) > (b)) ? (a) : (b))

#define REMOVE_SPACES(x) x.erase(std::remove(x.begin(), x.end(), ' '), x.end())

template <typename T, int N> T (&arrayTest(T(&)[N]))[N];
#define ARRAY_SIZE_IN_BYTES(array)  (sizeof(arrayTest(array)))


#define VPL_COMPUTE_GCC_VERSION(major, minor, patch) \
    (((((major) * 100) + minor)*10000) + patch)

#ifdef __GNUC__
#   define VPL_GCC_VERSION_NUMBER  VPL_COMPUTE_GCC_VERSION(__GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__)
#else
#   define VPL_GCC_VERSION_NUMBER  0
#endif

#if (VPL_GCC_VERSION_NUMBER >= VPL_COMPUTE_GCC_VERSION(3, 3, 0))
#   define ATTRIBUTE_NONNULL(...)  __attribute__((nonnull (__VA_ARGS__)))
#   define NO_NULL_ARGS  __attribute__((nonnull))
#else
#   define ATTRIBUTE_NONNULL(...)
#   define NO_NULL_ARGS
#endif


#if (VPL_GCC_VERSION_NUMBER >= VPL_COMPUTE_GCC_VERSION(3, 0, 0))
#   define ATTRIBUTE_PRINTF(fmtIdx, vaIdx) \
        __attribute__((format (printf, fmtIdx, vaIdx))) ATTRIBUTE_NONNULL(fmtIdx)
#else
#   define ATTRIBUTE_PRINTF(fmtIdx, vaIdx)
#endif

#define VPL_FALSE    false
#define VPL_TRUE    true
#define u8		unsigned char

#define FMTu_size_t "%lu"

#define PRI_UTLThreadId_t  PRIu64
#define FMT_UTLThreadId_t "%" PRI_UTLThreadId_t

#define PRIs64  PRIi64
#define FMTs64 "%" PRIs64
#define FMTu64 "%" PRIu64


#endif

