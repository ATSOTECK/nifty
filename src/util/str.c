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

#include "str.h"

#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>

bool isAlpha(const char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

bool isDigit(const char c) {
    return c >= '0' && c <= '9';
}

bool isOctDigit(const char c) {
    return (c >= '0' && c <= '7') || c == '_';
}

bool isHexDigit(const char c) {
    return (c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f') || c == '_';
}

bool isAlphaNumeric(const char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_' || (c >= '0' && c <= '9');
}

int str_len(const char *str) {
    register const char *s;
    for (s = str; *s; ++s) {}
    return (int)(s - str);
}

inline void str_cpy(register char *dst, register const char *src) {
    for (; (*dst = *src); ++src, ++dst);
}

inline void str_cpyn(register char *dst, const register int n, register const char *src) {
    for (int i = 0; i < n && ((*dst = *src)); ++src, ++dst, ++i) {}
    dst[n] = '\0';
}

char *str_new(const char *s, int *len) {
    const int length = str_len(s);
    char *ret = (char *)malloc(length * sizeof(char) + 1);
    str_cpy(ret, s);

    if (len != nullptr) {
        *len = length;
    }

    return ret;
}

char *str_new_len(const char *s, const int len) {
    char *ret = (char *)malloc(len * sizeof(char) + 1);
    str_cpyn(ret, len, s);

    return ret;
}

char *str_new_empty(const size_t size) {
    char *ret = (char *)malloc(size * sizeof(char) + 1);
    ret[0] = '\0';
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

bool str_eq2(const char *s1, const char *s2, const char *s3) {
    return str_eq(s1, s2) || str_eq(s1, s3);
}

bool str_eq_len(const char *s1, const char *s2, const int len) {
    if (s1 == nullptr || s2 == nullptr) {
        return false;
    }

    int i = 0;
    for (; i < len && *s1 == *s2; ++i, ++s1, ++s2) {}
    return i == len;
}

char *str_copy(char *dst, const char *src) {
    if (dst == nullptr || src == nullptr) {
        return nullptr;
    }

    const int srcLen = str_len(src);
    const int dstLen = str_len(dst);
    
    char *ret = dst;
    if (srcLen > dstLen) {
        ret = realloc(dst, sizeof(char) * srcLen + 1);
    }
    
    str_cpy(ret, src);
    return ret;
}

bool str_empty(const char *s) {
    return s == nullptr || str_len(s) == 0;
}

void str_clear(char *s) {
    if (str_empty(s)) {
        return;
    }

    s[0] = '\0';
}

void str_tolower(char *s) {
    if (str_empty(s)) {
        return;
    }

    for (char *c = s; *c != '\0'; ++c) {
        if (*c >= 'A' && *c <= 'Z') {
            *c |= 1 << 5;
        }
    }
}

void str_toupper(char *s) {
    if (str_empty(s)) {
        return;
    }

    for (char *c = s; *c != '\0'; ++c) {
        if (*c >= 'a' && *c <= 'z') {
            *c &= ~(1 << 5);
        }
    }
}

void str_clip_nl(char *s) {
    if (str_empty(s)) {
        return;
    }

    s[strcspn(s, "\r\n")] = '\0';
}

void str_fgets(char *s, const int len, FILE *f, const char *defaultAnswer) {
    if (s == nullptr) {
        return;
    }

    fgets(s, len, f);
    str_clip_nl(s);
    if (str_empty(s)) {
        if (defaultAnswer != nullptr) {
            str_cpyn(s, len, defaultAnswer);
        } else {
            str_empty(s);
        }
    }
}

char *str_get_line(const char *src, const int line, int *len) {
    if (str_empty(src)) {
        return nullptr;
    }

    int currentLine = 1;
    const char *at = src;
    for (; *at != EOF; ++at) {
        if (currentLine == line) {
            break;
        }

        if (*at == '\n') {
            ++currentLine;
        }
    }

    if (*at == EOF) {
        println("Could not find line %d.", line);
        return nullptr;
    }

    const char *start = at;
    int length = 0;
    for (; *at != '\n'; ++at, ++length) {}

    if (len != nullptr) {
        *len = length;
    }
    return str_new_len(start, length);
}

void println(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
    printf("\n");
}

void dbln() {
    printf("\n");
}

void printStrsWithSpacer(const char *s1, const char spacer, const char *s2, const int width) {
    if (s1 == nullptr) {
        return;
    }

    if (s2 == nullptr) {
        printf("%s\n", s1);
        return;
    }

    printf("%s ", s1);
    const int s1Len = str_len(s1);
    for (int i = 0; i < width - s1Len; ++i) {
        printf("%c", spacer);
    }
    printf(" %s\n", s2);
}

void setTextColor(const CompilerConfig *cfg, const char *color) {
    if (cfg == nullptr) {
        return;
    }

    if (!cfg->disableColors) {
        printf("%s", color);
    }
}
