//
// Created by Skyler Burwell on 6/5/25.
//

#ifndef UTIL_HPP
#define UTIL_HPP

#include <string>
#include <fmt/base.h>

template <typename... T>
void println(fmt::format_string<T...> fmt, T&&... args) {
    fmt::println(fmt, args...);
}
template <typename... T>
void print(fmt::format_string<T...> fmt, T&&... args) {
    fmt::print(fmt, args...);
}


void dbln();
void printStringsWithSpacer(const std::string &s1, char spacer, const std::string &s2, int width);

char *str_new_empty(size_t size);
char *str_new_fmt(int *len, const char *fmt, ...);
void str_delete(char *s);

bool str_eq(const char *s1, const char *s2);
int str_len(const char *str);

#endif //UTIL_HPP
