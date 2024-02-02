//
// Created by Skyler on 2/1/24.
//

#ifndef NIFTY_COMMON_H
#define NIFTY_COMMON_H

#define NIFTY_VERSION "0.0.1"
#define NIFTY_DATE "1 - February - 2024"
#define NIFTY_BUILD_FILE "build.toml"
#define NIFTY_ENTRY "__nifty_start"
#define NIFTY_GENERATED_FILE "_nifty_generated_c.c"

#ifndef __cplusplus
#   define nullptr ((void*)0)
#endif

typedef enum { false, true } bool;

#if defined(WIN32) || defined(_WIN32) || defined(_WIN64) || defined(__CYGWIN__)
#   define N_WIN
#   include <Windows.h>
#   undef max
#   undef min
#endif

#if defined(__APPLE__) || defined(__MACH__)
#   define N_MAC
#endif

#if defined(__linux__)
#   define N_LINUX
#endif

#if defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__) || defined(__DragonFly__)
#   define N_BSD
#endif

#endif //NIFTY_COMMON_H
