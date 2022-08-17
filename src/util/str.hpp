/*
 * Nifty - Nifty Programming Language
 * Copyright (c) 2022 Skyler Burwell
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

    keep size_t length() const;
    keep size_t size() const;

    void clear();
    keep bool empty() const;

    keep int64 indexOf(uchar32 cp) const;
    keep int64 indexOf(const String &str) const;
    keep int64 indexOfNext(uchar32 cp, size_t startFrom) const;
    keep int64 indexOfNext(const String &str, size_t startFrom) const;
    keep int64 indexOfLast(uchar32 cp) const;
    keep int64 indexOfLast(const String &str) const;
    keep bool contains(uchar32 cp) const;
    keep bool contains(const String &str) const;

    static bool isNumber(uchar32 cp);
    static bool isHexNumber(uchar32 cp);
    static bool isHexLetter(uchar32 cp);
    static bool isAlpha(uchar32 cp);
    static bool isLower(uchar32 cp);
    static bool isUpper(uchar32 cp);
    static bool isAlphanumeric(uchar32 cp);
    static bool isWhitespace(uchar32 cp);
    static bool isNewline(uchar32 cp);

    keep bool startsWith(const String &str, bool ignoreWhitespace = false) const;
    keep bool endsWith(const String &str, bool ignoreWhitespace = false) const;

    keep String stringAfter(size_t index) const;
    keep String stringBefore(size_t index) const;

    keep String stringAfterLast(uchar32 cp) const;
    keep String stringAfterLast(uchar32 cp0, uchar32 cp1) const;
    //This is NOT the same as (stringAfterLast(cp0) || stringAfterLast(cp1)).
    //Same for the following functions.

    keep String stringAfterFirst(uchar32 cp) const;
    keep String stringAfterFirst(uchar32 cp0, uchar32 cp1) const;

    keep String stringBeforeFirst(uchar32 cp) const;
    keep String stringBeforeFirst(uchar32 cp0, uchar32 cp1) const;

    keep String stringBeforeLast(uchar32 cp) const;
    keep String stringBeforeLast(uchar32 cp0, uchar32 cp1) const;

    void erase(size_t index, size_t count = 1);

    void trimLeadingWhitespace();
    void trimTrailingWhitespace();
    void trimWhitespace();

    void append(uchar32 cp, size_t buffSize = 0);
    void append(const String &string, size_t buffSize = 0);
    void prepend(uchar32 cp, size_t buffSize = 0);
    void prepend(const String &string, size_t buffSize = 0);
    void insert(uchar32 cp, size_t index);
    void insert(const String &str, size_t index);

    void replace(size_t index, size_t len, const String &str);
    bool replace(const String &findStr, const String &replaceStr);

    keep String substring(size_t index, size_t len = 0) const;
    keep String substr(size_t begin, size_t end) const;

    ///Only works with ASCII.
    keep String toUpper() const;
    keep String toLower() const;
    keep String swapCase() const;
    keep String reverse() const;

    keep char *c_str() const;
    keep std::string stdString() const;

    keep int toInt(int base = Dec) const;
    keep int128 toInt128(int base = Guess) const;
    keep float toFloat() const;
    keep double toDouble() const;

    void reserve(size_t size);
    void resize(size_t size);

    keep size_t offsetForCharIndex(size_t index) const;
    keep char raw(size_t index) const;
    keep uint32 codepoint(size_t index) const;
    static uchar32 codepointFor(const String &str);
    static String stringFrom(uchar32 cp);

    static String readFromFile(const String &path);

    operator std::string() const;

    String &operator =(const String &other);
    String &operator =(String &&other) noexcept;
    String &operator =(const char *other);
    String &operator =(const std::string &other);
    String &operator =(char other);
    String operator +(const String &other);
    String operator +(const char *other);
    String operator+=(const String &other);
    String operator+=(uchar32 cp);
    String operator+=(const char *other);
    String operator+=(const std::string &other);

    uchar32 operator[](size_t index) const;
    //uchar32 &operator[](size_t index);

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
    size_t _allocated; //Size in bytes of the memory allocated to the string.
    size_t _bsize; //Minimum number of bytes needed to represent the string.
    size_t _len; //Number of characters in the string.
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

#endif //__NIFTY_STRING_H__
