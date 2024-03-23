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

#ifndef NIFTY_STR_H
#define NIFTY_STR_H

#include "../common.h"

#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>

typedef char* string;
typedef const char* conststr;

bool isAlpha(char c);
bool isDigit(char c);
bool isOctDigit(char c);
bool isHexDigit(char c);
bool isAlphaNumeric(char c);

int str_len(conststr str);
void str_cpy(register string dst, register conststr src);
void str_cpyn(register string dst, register int n, register conststr src);

string str_new(conststr s, int *len);
string str_new_fmt(conststr s, int *len, conststr fmt, ...);
string str_new_empty(size_t size);

void str_delete(string s);

bool str_eq(conststr s1, conststr s2);
bool str_eq2(conststr s1, conststr s2, conststr s3);
bool str_eq_len(conststr s1, conststr s2, int len);

string str_copy(string dst, conststr src);

bool str_empty(conststr s);
void str_clear(string s);

void str_tolower(string s);
void str_toupper(string s);

void str_clip_nl(string s);

void str_fgets(string s, int len, FILE *f, conststr defaultAnswer);

void println(conststr fmt, ...);
void dbln();
void printStrsWithSpacer(conststr s1, char spacer, conststr s2, int width);

#endif //NIFTY_STR_H
