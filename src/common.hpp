//
// Created by Skyler Burwell on 6/5/25.
//

#ifndef COMMON_HPP
#define COMMON_HPP

#define NIFTY_VERSION "0.0.1"
#define NIFTY_DATE "5 - June - 2025"
#define NIFTY_BUILD_FILE "build.toml"
#define NIFTY_DEFAULT_PROJECT "project"

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

#define ERROR_COLOR      "\033[38;5;9m" // red
#define LINE_COLOR       "\033[38;5;2m" // green
#define WARN_COLOR       "\033[38;5;11m" // yellow
#define HIGHLIGHT_COLOR  "\033[38;5;12m" // blue
#define RESET_COLOR      "\033[0m"

enum class Verbosity {
    Debug,
    Info
};

struct CompilerConfig {
    bool disableColors;
    Verbosity verbosity;
};

#endif //COMMON_HPP
