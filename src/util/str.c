//
// Created by Skyler on 2/1/24.
//

#include "str.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

int str_len(conststr str) {
    register conststr s;
    for (s = str; *s; ++s);
    return (int)(s - str);
}

inline void str_cpy(register string dst, register conststr src) {
    for (; (*dst = *src); ++src, ++dst);
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
