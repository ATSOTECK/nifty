//
// Created by Skyler on 2/1/24.
//

#ifndef NIFTY_STR_H
#define NIFTY_STR_H

#include "../common.h"

#include <stddef.h>

char *str_new(const char *s, int *len);
char *str_new_empty(size_t size);

void str_delete(char *s);

bool str_eq(const char *s1, const char *s2);

int str_copy(char *dst, const char *src);

#endif //NIFTY_STR_H
