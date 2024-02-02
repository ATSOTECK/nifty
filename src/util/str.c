//
// Created by Skyler on 2/1/24.
//

#include "str.h"

#include <string.h>
#include <stdlib.h>

char *str_new(const char *s, int *len) {
    int length = (int)strlen(s);
    char *ret = (char*)malloc(length * sizeof(char) + 1);
    strcpy(ret, s);

    if (len != nullptr) {
        *len = length;
    }

    return ret;
}

char *str_new_empty(size_t size) {
    char *ret = (char*)malloc(size * sizeof(char) + 1);
    ret[0] = '\0';
    return ret;
}

void str_delete(char *s) {
    if (s == nullptr) {
        return;
    }

    free(s);
    s = nullptr;
}

bool str_eq(const char *s1, const char *s2) {
    while (*s1 == *s2++) {
        if (*s1++ == '\0') {
            return true;
        }
    }
    return false;
}

int str_copy(char *dst, const char *src) {
    return 0;
}
