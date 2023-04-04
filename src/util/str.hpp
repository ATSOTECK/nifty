/*
 * Nifty - Nifty Programming Language
 * Copyright (c) 2022 - 2023 Skyler Burwell
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

#ifndef __NIFTY_STRING_H__
#define __NIFTY_STRING_H__

#include "common.hpp"

#include <iostream>
#include <locale>
#include <string>

//TODO(Skyler): Add iterator.
class String {
public:
    String();
    String(const char *str);
    String(const std::string &str);
    String(const String &str);
    String(String &&str) noexcept;
    String(char c);
    String(int n);
    String(uint32 n);
    String(float n);
    String(double n);
    ~String() noexcept;

    [[nodiscard]] int64 length() const;
    [[nodiscard]] int64 size() const;

    void clear();
    [[nodiscard]] bool empty() const;

    [[nodiscard]] int64 indexOf(char32 cp) const;
    [[nodiscard]] int64 indexOf(const String &str) const;
    [[nodiscard]] int64 indexOfNext(char32 cp, int64 startFrom) const;
    [[nodiscard]] int64 indexOfNext(const String &str, int64 startFrom) const;
    [[nodiscard]] int64 indexOfLast(char32 cp) const;
    [[nodiscard]] int64 indexOfLast(const String &str) const;
    [[nodiscard]] bool contains(char32 cp) const;
    [[nodiscard]] bool contains(const String &str) const;

    static bool isNumber(char32 cp);
    static bool isHexNumber(char32 cp);
    static bool isHexLetter(char32 cp);
    static bool isAlpha(char32 cp);
    static bool isLower(char32 cp);
    static bool isUpper(char32 cp);
    static bool isAlphanumeric(char32 cp);
    static bool isWhitespace(char32 cp);
    static bool isNewline(char32 cp);

    [[nodiscard]] bool startsWith(const String &str, bool ignoreWhitespace = false) const;
    [[nodiscard]] bool endsWith(const String &str, bool ignoreWhitespace = false) const;

    [[nodiscard]] String stringAfter(int64 index) const;
    [[nodiscard]] String stringBefore(int64 index) const;

    [[nodiscard]] String stringAfterLast(char32 cp) const;
    [[nodiscard]] String stringAfterLast(char32 cp0, char32 cp1) const;
    //This is NOT the same as (stringAfterLast(cp0) || stringAfterLast(cp1)).
    //Same for the following functions.

    [[nodiscard]] String stringAfterFirst(char32 cp) const;
    [[nodiscard]] String stringAfterFirst(char32 cp0, char32 cp1) const;

    [[nodiscard]] String stringBeforeFirst(char32 cp) const;
    [[nodiscard]] String stringBeforeFirst(char32 cp0, char32 cp1) const;

    [[nodiscard]] String stringBeforeLast(char32 cp) const;
    [[nodiscard]] String stringBeforeLast(char32 cp0, char32 cp1) const;

    void erase(int64 index, int64 count = 1);

    void trimLeadingWhitespace();
    void trimTrailingWhitespace();
    void trimWhitespace();

    void append(char32 cp, int64 buffSize = 0);
    void append(const String &string, int64 buffSize = 0);
    void prepend(char32 cp, int64 buffSize = 0);
    void prepend(const String &string, int64 buffSize = 0);
    void insert(char32 cp, int64 index);
    void insert(const String &str, int64 index);

    void replace(int64 index, int64 len, const String &str);
    bool replace(const String &findStr, const String &replaceStr);

    [[nodiscard]] String substring(int64 index, int64 len = 0) const;
    [[nodiscard]] String substr(int64 begin, int64 end) const;

    ///Only works with ASCII.
    [[nodiscard]] String toUpper() const;
    [[nodiscard]] String toLower() const;
    [[nodiscard]] String swapCase() const;
    [[nodiscard]] String reverse() const;

    [[nodiscard]] char *c_str() const;
    [[nodiscard]] std::string stdString() const;

    [[nodiscard]] int toInt(int base = Dec) const;
    [[nodiscard]] float toFloat() const;
    [[nodiscard]] double toDouble() const;

    void reserve(int64 size);
    void resize(int64 size);

    [[nodiscard]] int64 offsetForCharIndex(int64 index) const;
    [[nodiscard]] char raw(int64 index) const;
    [[nodiscard]] int32 codepoint(int64 index) const;
    static int32 codepointFor(const String &str);
    static String stringFrom(char32 cp);

    static String readFromFile(const String &path);

    operator std::string() const;

    String &operator =(const String &other);
    String &operator =(String &&other) noexcept;
    String &operator =(const char *other);
    String &operator =(const std::string &other);
    String &operator =(char32 other);
    String operator +(const String &other);
    String operator +(const char *other);
    String operator+=(const String &other);
    String operator+=(char32 cp);
    String operator+=(const char *other);
    String operator+=(const std::string &other);

    char32 operator[](int64 index) const;
    //char32 &operator[](int64 index);

    enum Base {
        Guess = 0,
        Bin = 2,
        Oct = 8,
        Dec = 10,
        Hex = 16
    };

private:
    void init(const char *str);
    void reset();

    friend bool operator==(const String &lhs, const String &rhs);
    friend bool operator==(const String &lhs, const char *rhs);

    friend std::ostream &operator <<(std::ostream &os, String &str);
    friend std::ostream &operator <<(std::ostream &os, const String &str);

    char *_str;
    int64 _allocated; //Size in bytes of the memory allocated to the string.
    int64 _bsize; //Minimum number of bytes needed to represent the string.
    int64 _len; //Number of characters in the string.
};

String operator+(char *lhs, const String &rhs);
String operator+(const std::string &lhs, const String &rhs);

bool operator==(const String &lhs, const String &rhs);
bool operator!=(const String &lhs, const String &rhs);

bool operator ==(const String &lhs, const char *rhs);
bool operator !=(const String &lhs, const char *rhs);

std::ostream &operator <<(std::ostream &os, String &str);
std::ostream &operator <<(std::ostream &os, const String &str);

bool operator <(const String &lhs, const String &rhs);
bool operator >(const String &lhs, const String &rhs);

namespace std {
template<>
class hash<String> {
public:
    std::uint64_t operator() (const String &str) const {
        uint64_t ret = 2166136261u;
        for (int i = 0; i < str.size(); ++i) {
            ret ^= (uint8_t)str.raw(i);
            ret *= 16777619;
        }

        return ret;
    }
};
}

#include <vector>
typedef std::vector<String> Strings;

#endif //__NIFTY_STRING_H__
