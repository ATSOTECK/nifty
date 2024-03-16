//
// Created by Skyler on 2/1/24.
//

#ifndef NIFTY_STR_H
#define NIFTY_STR_H

#include "../common.h"

#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>

int str_len(conststr str);
void str_cpy(register string dst, register conststr src);
void str_cpyn(register string dst, register int n, register conststr src);

string str_new(conststr s, int *len);
string str_new_fmt(conststr s, int *len, conststr fmt, ...);
string str_new_empty(size_t size);

void str_delete(string s);

bool str_eq(conststr s1, conststr s2);
bool str_eq2(conststr s1, conststr s2, conststr s3);

string str_copy(string dst, conststr src);

bool str_empty(conststr s);
void str_clear(string s);

void str_tolower(string s);
void str_toupper(string s);

void str_clip_nl(string s);

void str_fgets(string s, int len, struct __sFILE *f, conststr defaultAnswer);

void println(conststr fmt, ...);
void dbln();
void printStrsWithSpacer(conststr s1, char spacer, conststr s2, int width);

#endif //NIFTY_STR_H
