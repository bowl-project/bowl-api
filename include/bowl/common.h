#ifndef COMMON_H
#define COMMON_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <inttypes.h>
#include <ctype.h>
#include <math.h>

typedef uint64_t u64;
typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t  u8;

typedef int64_t s64;
typedef int32_t s32;
typedef int16_t s16;
typedef int8_t  s8;

#if _WIN64 || (__GNUC__ && (__x86_64__ || __ppc64__))
#define OS_ARCHITECTURE_64
#define OS_ARCHITECTURE "64-bit"
#else 
#define OS_ARCHITECTURE_32
#define OS_ARCHITECTURE "32-bit"
#endif

#if __APPLE__ || __MACH__
#define OS_UNIX
#define OS_NAME "Mac OS X"
#elif _WIN32 || _WIN64
#define OS_WINDOWS
#define OS_NAME "Windows"
#elif __linux__
#define OS_UNIX
#define OS_NAME "Linux"
#elif __FreeBSD__
#define OS_UNIX
#define OS_NAME "FreeBSD"
#elif unix || __unix || __unix__
#define OS_UNIX
#define OS_NAME "Unix"
#else
#define OS_NAME "Unknown"
#endif

#if defined(OS_UNIX)
#include <dlfcn.h>
#elif defined(OS_WINDOWS)
#include <windows.h>
#endif

/**
 * A helper for the 'CONCAT' macro. 
 * @internal
 */
#define _CONCAT(x, y) x ## y

/**
 * Concatenates the two provided arguments.
 * @param x The first argument.
 * @param y The second argument.
 * @return The concatenation of 'x' and 'y'.
 */
#define CONCAT(x, y) _CONCAT(x, y)

#endif
