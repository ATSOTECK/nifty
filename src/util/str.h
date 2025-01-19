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

#include <stddef.h>
#include <stdio.h>

bool isAlpha(char c);
bool isDigit(char c);
bool isOctDigit(char c);
bool isHexDigit(char c);
bool isAlphaNumeric(char c);

int str_len(const char *str);
void str_cpy(register char *dst, register const char *src);
void str_cpyn(register char *dst, register int n, register const char *src);

char *str_new(const char *s, int *len);
char *str_new_len(const char *s, int len);
char *str_new_fmt(int *len, const char *fmt, ...);
char *str_new_empty(size_t size);

void str_delete(char *s);

bool str_eq(const char *s1, const char *s2);
bool str_eq2(const char *s1, const char *s2, const char *s3);
bool str_eq_len(const char *s1, const char *s2, int len);

char *str_copy(char *dst, const char *src);

bool str_empty(const char *s);
void str_clear(char *s);

void str_tolower(char *s);
void str_toupper(char *s);

void str_clip_nl(char *s);

void str_fgets(char *s, int len, FILE *f, const char *defaultAnswer);

char * str_get_line(const char *src, int line, int *len);

void println(const char *fmt, ...);
void dbln();
void printStrsWithSpacer(const char *s1, char spacer, const char *s2, int width);

void setTextColor(const CompilerConfig *cfg, const char *color);

#endif //NIFTY_STR_H
