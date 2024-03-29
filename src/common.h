/*
 * Nifty - Nifty Programming Language
 * Copyright (c) 2024 Skyler Burwell
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 */

#ifndef NIFTY_COMMON_H
#define NIFTY_COMMON_H

#define NIFTY_VERSION "0.0.1"
#define NIFTY_DATE "28 - March - 2024"
#define NIFTY_BUILD_FILE "build.toml"
#define NIFTY_ENTRY "__nifty_start"
#define NIFTY_GENERATED_FILE "_nifty_generated_c.c"

#ifndef __cplusplus
#   define nullptr ((void*)0)
#endif

typedef enum { false, true } bool;

typedef enum {
    Debug,
    Info
} Verbosity;

typedef struct {
    bool disableColors;
    Verbosity verbosity;
} CompilerConfig;

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

#ifdef N_WIN
#   include <io.h>
#   define F_OK 0
#   define access _access
#endif

#define ERROR_COLOR      "\033[38;5;9m" // red
#define LINE_COLOR       "\033[38;5;2m" // green
#define WARN_COLOR       "\033[38;5;11m" // yellow
#define HIGHLIGHT_COLOR  "\033[38;5;12m" // blue
#define RESET_COLOR      "\033[0m"

#ifndef max
#   define max(a,b) (((a) > (b)) ? (a) : (b))
#endif

#endif //NIFTY_COMMON_H
