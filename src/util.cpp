//
// Created by Skyler Burwell on 6/5/25.
//

#include "util.hpp"

void dbln() {
    fmt::println("");
}

void printStringsWithSpacer(const std::string &s1, char spacer, const std::string &s2, const int width) {
    if (s1.empty()) {
        return;
    }

    if (s2.empty()) {
        fmt::println("{}", s1);
        return;
    }

    fmt::print("{} ", s1);
    for (int i = 0; i < width - s1.length(); ++i) {
        fmt::print("{}", spacer);
    }
    fmt::println(" {}", s2);
}

char *str_new_empty(const size_t size) {
    char *ret = (char *)malloc(size * sizeof(char) + 1);
    ret[0] = '\0';
    return ret;
}

char *str_new_fmt(int *len, const char *fmt, ...) {
    char *ret = (char *)malloc(2048);
    va_list args;
    va_start(args, fmt);
    vsprintf(ret, fmt, args);
    va_end(args);

    if (len != nullptr) {
        *len = str_len(ret);
    }

    return ret;
}

void str_delete(char *s) {
    if (s == nullptr) {
        return;
    }

    free(s);
}

bool str_eq(const char *s1, const char *s2) {
    if (s1 == s2) {
        return true;
    }

    if (s1 == nullptr || s2 == nullptr) {
        return false;
    }

    while (*s1 == *s2++) {
        if (*s1++ == '\0') {
            return true;
        }
    }

    return false;
}

int str_len(const char *str) {
    const char *s;
    for (s = str; *s; ++s) {}
    return (int)(s - str);
}
