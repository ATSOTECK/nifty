//
// Created by Skyler on 2/1/24.
//

#include "str.h"

#include <string.h>
#include <stdlib.h>

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
    s = nullptr;
}

bool str_eq(conststr s1, conststr s2) {
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
