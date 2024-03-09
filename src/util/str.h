//
// Created by Skyler on 2/1/24.
//

#ifndef NIFTY_STR_H
#define NIFTY_STR_H

#include "../common.h"

#include <stddef.h>

int str_len(conststr str);
void str_cpy(register string dst, register conststr src);

string str_new(conststr s, int *len);
string str_new_empty(size_t size);

void str_delete(string s);

bool str_eq(conststr s1, conststr s2);
bool str_eq2(conststr s1, conststr s2, conststr s3);

string str_copy(string dst, conststr src);

#endif //NIFTY_STR_H
