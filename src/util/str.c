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
#include <stdlib.h>
#include <stdio.h>

bool isAlpha(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

bool isDigit(char c) {
    return c >= '0' && c <= '9';
}

bool isOctDigit(char c) {
    return ((c >= '0' && c <= '7') || (c == '_'));
}

bool isHexDigit(char c) {
    return ((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f') || (c == '_'));
}

bool isAlphaNumeric(char c) {
    return ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_') || (c >= '0' && c <= '9');
}

int str_len(conststr str) {
    register conststr s;
    for (s = str; *s; ++s);
    return (int)(s - str);
}

inline void str_cpy(register string dst, register conststr src) {
    for (; (*dst = *src); ++src, ++dst);
}

inline void str_cpyn(register string dst, register int n, register conststr src) {
    for (int i = 0; i < n && (*dst = *src); ++src, ++dst, ++i);
    dst[n] = '\0';
}

string str_new(conststr s, int *len) {
    int length = str_len(s);
    char *ret = (string)malloc(length * sizeof(char) + 1);
    str_cpy(ret, s);

    if (len != nullptr) {
        *len = length;
    }

    return ret;
}

string str_new_empty(size_t size) {
    string ret = (string)malloc(size * sizeof(char) + 1);
    ret[0] = '\0';
    return ret;
}

void str_delete(string s) {
    if (s == nullptr) {
        return;
    }

    free(s);
}

bool str_eq(conststr s1, conststr s2) {
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

bool str_eq2(conststr s1, conststr s2, conststr s3) {
    return str_eq(s1, s2) || str_eq(s1, s3);
}

string str_copy(string dst, conststr src) {
    int srcLen = str_len(src);
    int dstLen = str_len(dst);
    
    string ret = dst;
    if (srcLen > dstLen) {
        ret = realloc(dst, sizeof(char) * srcLen + 1);
    }
    
    str_cpy(ret, src);
    return ret;
}

bool str_empty(conststr s) {
    return s == nullptr || str_len(s) == 0;
}

void str_clear(string s) {
    if (str_empty(s)) {
        return;
    }

    s[0] = '\0';
}

void str_tolower(string s) {
    if (str_empty(s)) {
        return;
    }

    for (char *c = s; *c != '\0'; ++c) {
        if (*c >= 'A' && *c <= 'Z') {
            *c |= (1 << 5);
        }
    }
}

void str_toupper(string s) {
    if (str_empty(s)) {
        return;
    }

    for (char *c = s; *c != '\0'; ++c) {
        if (*c >= 'a' && *c <= 'z') {
            *c &= ~(1 << 5);
        }
    }
}

void str_clip_nl(string s) {
    if (str_empty(s)) {
        return;
    }

    s[strcspn(s, "\r\n")] = '\0';
}

void str_fgets(string s, int len, FILE *f, conststr defaultAnswer) {
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

void println(conststr fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
    printf("\n");
}

void dbln() {
    printf("\n");
}

void printStrsWithSpacer(conststr s1, char spacer, conststr s2, int width) {
    if (s1 == nullptr) {
        return;
    }

    if (s2 == nullptr) {
        printf("%s\n", s1);
        return;
    }

    printf("%s ", s1);
    int s1Len = str_len(s1);
    for (int i = 0; i < width - s1Len; ++i) {
        printf("%c", spacer);
    }
    printf(" %s\n", s2);
}
