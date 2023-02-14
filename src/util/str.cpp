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

#include "str.hpp"

#include <fstream>

#ifndef VWIN
namespace {

    inline void strcpy_s(char *dst, size_t dstsz, const char *src) {
        strncpy(dst, src, dstsz);
    }

    inline void strcat_s(char *dst, size_t dstsz, const char *src) {
        strncat(dst, src, dstsz);
    }

}
#endif //VWIN

int64 strsize(const char *str) {
    int64 size = 1;
    while (*str++ != '\0') {
        ++size;
    }

    return size;
}

int64 utf8_strlen(const char *str) {
    if (!str) {
        return 0;
    }

    int64 c, i, l;

    for (l = 0, i = 0; i < strsize(str) - 1; ++i, ++l) {
        c = (unsigned char)str[i];
        if (c >= 0 && c <= 127) {
            i += 0;
        } else if ((c & 0xE0u) == 0xC0) {
            i += 1;
        } else if ((c & 0xF0u) == 0xE0) {
            i += 2;
        } else if ((c & 0xF8u) == 0xF0) {
            i += 3;
        } else {
            return 0;
        }
    }

    return l;
}

size_t strcmpv(const char *lhs, const char *rhs) {
    size_t ls = strsize(lhs);
    size_t rs = strsize(rhs);

    if (ls < rs) {
        return -1;
    } else if (rs > ls) {
        return 1;
    }

    size_t diff = 0;
    for (size_t i = 0; i < ls; ++i) {
        if (*lhs++ != *rhs++) {
            ++diff;
        }
    }

    return diff;
}

size_t strcmpcnt(const char *lhs, size_t count, const char *rhs) {
    size_t diff = 0;
    for (size_t i = 0; i < count; ++i) {
        if (*lhs++ != *rhs++) {
            ++diff;
        }
    }

    return diff;
}

char *cptocstr(char32 cp, int &size) {
    char *str = nullptr;

    if (cp <= 0x7F) {
        size = 2;
        str = new char[size];
        str[0] = (char)cp;
        str[1] = '\0';
    } else if (cp <= 0x7FF) {
        size = 3;
        str = new char[size];
        str[0] = (char)(((cp >> 6u) & 0x1Fu) | 0xC0u);
        str[1] = (char)(((cp >> 0u) & 0x3Fu) | 0x80u);
        str[2] = '\0';
    } else if (cp <= 0xFFFF) {
        size = 4;
        str = new char[size];
        str[0] = (char)(((cp >> 12u) & 0x0Fu) | 0xE0u);
        str[1] = (char)(((cp >>  6u) & 0x3Fu) | 0x80u);
        str[2] = (char)(((cp >>  0u) & 0x3Fu) | 0x80u);
        str[3] = '\0';
    } else if (cp <= 0x10FFFF) {
        size = 5;
        str = new char[size];
        str[0] = (char)(((cp >> 18u) & 0x07u) | 0xF0u);
        str[1] = (char)(((cp >> 12u) & 0x3Fu) | 0x80u);
        str[2] = (char)(((cp >>  6u) & 0x3Fu) | 0x80u);
        str[3] = (char)(((cp >>  0u) & 0x3Fu) | 0x80u);
        str[4] = '\0';
    } else {
        size = 0;
    }

    --size;
    return str;
}

std::string cptostd(char32 cp) {
    int size;
    std::string ret = cptocstr(cp, size);

    return ret;
}

String::String():
        _str(nullptr),
        _allocated(0),
        _bsize(0),
        _len(0)
{
    init(nullptr);
}

String::String(const char *str):
        _str(nullptr),
        _allocated(0),
        _bsize(0),
        _len(0)
{
    init(str);
}

String::String(const std::string &str):
        _str(nullptr),
        _allocated(0),
        _bsize(0),
        _len(0)
{
    init(str.c_str());
}

String::String(const String &str):
        _str(nullptr),
        _allocated(0),
        _bsize(0),
        _len(0)
{
    init(str._str);
}

String::String(String &&str) noexcept :
        _str(str._str),
        _allocated(str._allocated),
        _bsize(str._bsize),
        _len(str._len)
{
    str._str = nullptr;
    str._allocated = 0;
    str._bsize = 0;
    str._len = 0;
}

String::String(char c):
    _str(nullptr),
    _allocated(0),
    _bsize(0),
    _len(0)
{
    init(std::string(1, c).c_str());
}

String::String(int n):
        _str(nullptr),
        _allocated(0),
        _bsize(0),
        _len(0)
{
    init(std::to_string(n).c_str());
}

String::String(uint32 n):
        _str(nullptr),
        _allocated(0),
        _bsize(0),
        _len(0)
{
    init(std::to_string(n).c_str());
}

String::String(float n):
        _str(nullptr),
        _allocated(0),
        _bsize(0),
        _len(0)
{
    init(std::to_string(n).c_str());
}

String::String(double n):
        _str(nullptr),
        _allocated(0),
        _bsize(0),
        _len(0)
{
    init(std::to_string(n).c_str());
}

String::~String() noexcept {
    delete[] _str;
}

void String::init(const char *str) {
    _str = nullptr;

    if (str != nullptr) {
        _allocated = strsize(str);
        if (_allocated > 1) {
            _str = new char[_allocated];
            strcpy_s(_str, _allocated, str);
            _len = utf8_strlen(_str);
            _bsize = strsize(_str);
            return;
        }
    }

    reset();
}

void String::reset() {
    if (_str != nullptr) {
        delete[] _str;
        _str = nullptr;
    }

    reserve(1);
}

int64 String::length() const {
    return _len;
}

int64 String::size() const {
    return _allocated;
}

void String::clear() {
    reset();
}

bool String::empty() const {
    return (_str == nullptr || _len == 0);
}

int64 String::indexOf(char32 cp) const {
    return indexOfNext(cp, 0);
}

int64 String::indexOfNext(char32 cp, int64 startFrom) const {
    for (int64 i = startFrom; i <= _len; ++i) {
        if (cp == codepoint(i)) {
            return i;
        }
    }

    return -1;
}

int64 String::indexOfLast(char32 cp) const {
    for (int64 i = _len - 1; i >= 0; --i) {
        if (cp == codepoint(i)) {
            return i;
        }
    }

    return -1;
}

bool String::contains(char32 cp) const {
    return (indexOfNext(cp, 0) != -1);
}

bool String::isNumber(char32 cp) {
    return (cp >= '0' && cp <= '9');
}

bool String::isHexNumber(char32 cp) {
    return ((cp >= '0' && cp <= '9') || (cp >= 'a' && cp <= 'f') || (cp >= 'A' && cp <= 'F'));
}

bool String::isHexLetter(char32 cp) {
    return ((cp >= 'a' && cp <= 'f') || (cp >= 'A' && cp <= 'F'));
}

bool String::isAlpha(char32 cp) {
    return ((cp >= 'a' && cp <= 'z') || (cp >= 'A' && cp <= 'Z'));
}

bool String::isLower(char32 cp) {
    return (cp >= 'a' && cp <= 'z');
}

bool String::isUpper(char32 cp) {
    return (cp >= 'A' && cp <= 'Z');
}

bool String::isAlphanumeric(char32 cp) {
    return (isAlpha(cp) || isNumber(cp));
}

bool String::isWhitespace(char32 cp) {
    return (cp == ' ' || cp == '\t' || cp == '\n' || cp == '\r');
}

bool String::isNewline(char32 cp) {
    return (cp == '\n');
}

bool String::startsWith(const String &str, bool ignoreWhitespace) const {
    size_t i = 0;
    size_t s = 0;

    if (ignoreWhitespace) {
        while (isWhitespace(codepoint(i))) {++i;}
    }

    return (strcmpcnt(_str + i, str._bsize - 1, str._str + s) == 0);
}

bool String::endsWith(const String &str, bool ignoreWhitespace) const {
    if (ignoreWhitespace) {
        //TODO(Skyler): Redo this so it doesn't have to copy the string.
        String tmp = *this;
        tmp.trimWhitespace();

        return tmp.endsWith(str, false);
    }

    return (strcmpcnt(_str + (_bsize - str._bsize), str._bsize - 1, str._str) == 0);
}

String String::stringAfter(int64 index) const {
    String ret;

    if (index >= _len) {
        return ret;
    }

    size_t offset = offsetForCharIndex(index + 1);

    ret = (_str + offset);
    return ret;
}

String String::stringBefore(int64 index) const {
    String ret;

    if (index <= 0) {
        return ret;
    }

    int64 size = offsetForCharIndex(index);
    ret.resize(size + 1);
    memcpy(ret._str, _str, size * sizeof(char));
    ret._str[size] = '\0';
    ret._bsize = ret._allocated;
    ret._len = utf8_strlen(ret._str);

    return ret;
}

String String::stringAfterLast(char32 cp) const {
    return stringAfter(indexOfLast(cp));
}

String String::stringAfterLast(char32 cp0, char32 cp1) const {
    String ret;
    size_t offset;
    char32 cp;

    for (int64 i = _len; i >= 0; --i) {
        cp = codepoint(i);
        if (cp == cp0 || cp == cp1) {
            if (i == _len) {
                return ret;
            }

            offset = offsetForCharIndex(i + 1);
            ret = (_str + offset);

            return ret;
        }
    }

    return ret;
}

String String::stringAfterFirst(char32 cp) const {
    return stringAfter(indexOf(cp));
}

String String::stringAfterFirst(char32 cp0, char32 cp1) const {
    String ret;
    size_t offset;
    char32 cp;

    for (int64 i = 0; i <= _len; ++i) {
        cp = codepoint(i);
        if (cp == cp0 || cp == cp1) {
            if (i == _len) {
                return ret;
            }

            offset = offsetForCharIndex(i + 1);
            ret = (_str + offset);

            return ret;
        }
    }

    return ret;
}

String String::stringBeforeFirst(char32 cp) const {
    return stringBefore(indexOf(cp));
}

String String::stringBeforeFirst(char32 cp0, char32 cp1) const {
    String ret;
    char32 cp;

    for (int64 i = 0; i <= _len; ++i) {
        cp = codepoint(i);
        if (cp == cp0 || cp == cp1) {
            if (i == 0) {
                return ret;
            }

            return stringBefore(i);
        }
    }

    return ret;
}

String String::stringBeforeLast(char32 cp) const {
    return stringBefore(indexOfLast(cp));
}

String String::stringBeforeLast(char32 cp0, char32 cp1) const {
    String ret;
    char32 cp;

    for (int64 i = _len; i >= 0; --i) {
        cp = codepoint(i);
        if (cp == cp0 || cp == cp1) {
            if (i == 0) {
                return ret;
            }

            return stringBefore(i);
        }
    }

    return ret;
}

void String::erase(int64 index, int64 count) {
    if (_len == 0 || count == 0 || index + count > _len) {
        return;
    }

    int64 end = offsetForCharIndex(index + count);
    index = offsetForCharIndex(index);

    memcpy(_str + index, _str + end, _allocated - end);
    _str[offsetForCharIndex(_len) - ((index - end) + 1)] = '\0';
    _len = utf8_strlen(_str);
    _bsize = strsize(_str);
}

void String::trimLeadingWhitespace() {
    int64 i = 0;
    while (isWhitespace(codepoint(i)) && i <= _len) {
        ++i;
    }

    if (i != 0) {
        erase(0, i);
    }
}

void String::trimTrailingWhitespace() {
    int i = 0;
    while (isWhitespace(codepoint(_len + i - 1)) && i >= 0) {
        --i;
    }

    if (i != 0) {
        erase(_len + i, -i);
    }
}

void String::trimWhitespace() {
    trimLeadingWhitespace();
    trimTrailingWhitespace();
}

void String::append(char32 cp, int64 buffSize) {
    int cps;
    char *c = cptocstr(cp, cps);

    if (buffSize > 0) {
        resize(_allocated + buffSize);
    } else if (_bsize + cps > _allocated) {
        resize(_allocated + cps);
    }

    if (_len == 0) {
        strcpy_s(_str, cps + 1, c);
        _bsize = cps + 1;
    } else {
        strcat_s(_str, _bsize + cps, c);
        _bsize += cps;
    }

    ++_len;
}

void String::append(const String &string, int64 buffSize) {
    if (buffSize > 0 && buffSize > string._bsize) {
        resize(_allocated + buffSize);
    } else if (_bsize + string._bsize > _allocated) {
        resize(_allocated + string._bsize);
    }

    if (_len == 0) {
        strcpy_s(_str, string._bsize, string._str);
        _bsize = string._bsize;
    } else {
        strcat_s(_str, _bsize + string._bsize, string._str);
        _bsize += string._bsize;
    }

    _len += string._len;
}

String String::substring(int64 index, int64 len) const {
    if (len == 0) {
        return substr(index, offsetForCharIndex(_len));
    }

    return substr(index, index + len);
}

String String::substr(int64 begin, int64 end) const {
    begin = offsetForCharIndex(begin);
    end = offsetForCharIndex(end);

    String ret;

    int64 size = end - begin + 1;
    ret.resize(size + 1);
    strcpy_s(ret._str, size, _str + begin);
    ret._str[size] = '\0';
    ret._bsize = strsize(ret._str);
    ret._len = utf8_strlen(ret._str);

    return ret;
}

String String::toUpper() const {
    String ret = *this;

    for (int64 i = 0; i < _len; ++i) {
        if (isLower(ret[i])) {
            ret._str[ret.offsetForCharIndex(i)] -= 32;
        }
    }

    return ret;
}

String String::toLower() const {
    String ret = *this;

    for (int64 i = 0; i < _len; ++i) {
        if (isUpper(ret[i])) {
            ret._str[ret.offsetForCharIndex(i)] += 32;
        }
    }

    return ret;
}

String String::swapCase() const {
    String ret = *this;

    for (int64 i = 0; i < _len; ++i) {
        char32 cp = ret[i];
        if (isUpper(cp)) {
            ret._str[ret.offsetForCharIndex(i)] += 32;
        } else if (isLower(cp)) {
            ret._str[ret.offsetForCharIndex(i)] -= 32;
        }
    }

    return ret;
}

String String::reverse() const {
    String ret;
    ret.reserve(_allocated);
    //TODO(Skyler): Make work.
    return ret;
}

char *String::c_str() const {
    return _str;
}

std::string String::stdString() const {
    return {_str};
}

int String::toInt(int base) const {
    return (int)strtol(_str, nullptr, base);
}

float String::toFloat() const {
    return (float)strtod(_str, nullptr);
}

double String::toDouble() const {
    return strtod(_str, nullptr);
}

void String::reserve(int64 size) {
    if (_str != nullptr) {
        return;
    }

    _allocated = size;
    _bsize = 1;
    _len = 0;
    _str = new char[size];
    _str[0] = '\0';
}

void String::resize(int64 size) {
    if (size == 0 || size < _allocated) {
        return;
    }

    if (_allocated == 0) {
        _allocated = size + 1;
        _str = new char[_allocated];
        memset(_str, 0, _allocated);
        _str[0] = '\0';
        _bsize = 1;
        _len = 0;
    } else if (_allocated < size) {
        _allocated = size;

        char *newStr = new char[_allocated];

        if (_len != 0) {
            strcpy_s(newStr, _bsize, _str);
        }

        delete[] _str;
        _str = newStr;

        if (_len == 0) {
            _str[0] = '\0';
        }
    }
}

int64 String::offsetForCharIndex(int64 index) const {
    if (!_str || index > _len || index == 0) {
        return 0;
    }

    int64 c, i, l;

    for (l = 0, i = 0; i < _bsize - 1; ++i, ++l) {
        c = (unsigned char)_str[i];
        if (c >= 0 && c <= 127) {
            i += 0;
        } else if ((c & 0xE0u) == 0xC0) {
            i += 1;
        } else if ((c & 0xF0u) == 0xE0) {
            i += 2;
        } else if ((c & 0xF8u) == 0xF0) {
            i += 3;
        } else {
            return 0;
        }

        if (index == l + 1) {
            return i + 1;
        }
    }

    return 0;
}

char String::raw(int64 index) const {
    if (index >= _bsize) {
        return '\0';
    }

    return _str[index];
}

int32 String::codepoint(int64 index) const {
    if (index > _len) {
        return 0;
    }

    char32 cp = 0;
    index = offsetForCharIndex(index);
    char32 c = (uchar)_str[index];

    if ((c & 0x80u) == 0) {
        cp = (unsigned char)c;
    } else if ((c & 0xE0u) == 0xC0) {
        cp =  ((int32)_str[index]     & 0x1F) << 6u;
        cp |= ((int32)_str[index + 1] & 0x3F);
    } else if ((c & 0xF0u) == 0xE0) {
        cp =  ((int32)_str[index]     & 0xF)  << 12u;
        cp |= ((int32)_str[index + 1] & 0x3F) << 6u;
        cp |= ((int32)_str[index + 2] & 0x3F);
    } else if ((c & 0xF8u) == 0xF0) {
        cp =  ((int32)_str[index]     & 0x7)  << 18u;
        cp |= ((int32)_str[index + 1] & 0x3F) << 12u;
        cp |= ((int32)_str[index + 2] & 0x3F) << 6u;
        cp |= ((int32)_str[index + 3] & 0x3F);
    } else if ((c & 0xFCu) == 0xF8) {
        cp =  ((int32)_str[index]     & 0x3)  << 24u;
        cp |= ((int32)_str[index]     & 0x3F) << 18u;
        cp |= ((int32)_str[index]     & 0x3F) << 12u;
        cp |= ((int32)_str[index]     & 0x3F) << 6u;
        cp |= ((int32)_str[index]     & 0x3F);
    } else if ((c & 0xFEu) == 0xFC) {
        cp =  ((int32)_str[index]     & 0x1)  << 30u;
        cp |= ((int32)_str[index]     & 0x3F) << 24u;
        cp |= ((int32)_str[index]     & 0x3F) << 18u;
        cp |= ((int32)_str[index]     & 0x3F) << 12u;
        cp |= ((int32)_str[index]     & 0x3F) << 6u;
        cp |= ((int32)_str[index]     & 0x3F);
    }

    return cp;
}

char32 String::codepointFor(const String &str) {
    if (str._len != 1) {
        vwarn("Expected string length of 1, got length of " << str._len);

        if (str._len == 0) {
            return 0;
        }
    }

    return str.codepoint(0);
}

String String::stringFrom(char32 cp) {
    int size;
    return {cptocstr(cp, size)};
}

String String::readFromFile(const String &path) {
    String str;

    std::ifstream file;
    file.open(path.stdString(), std::ios_base::in);

    std::string line;
    if (file.is_open()) {
        //This method uses less memory for whatever reason.
        int64 fileSize = 0;
        until(file.get() == EOF) {
            ++fileSize;
        }
        file.clear();
        file.seekg(0, std::ios_base::beg);

        str.resize(fileSize + 1);
        file.read(str._str, fileSize);
        file.close();

        str._str[fileSize] = '\0';
        str._bsize = strsize(str._str);
        str._len = utf8_strlen(str._str);

        /*
        while (std::getline(file, line)) {
            str += line + "\n";
        }
        file.close();//*/
    } else {
        std::cout << "Error unable to load \"" << path << "\"\n";
    }

    return str;
}

String::operator std::string() const {
    return stdString();
}

String &String::operator =(const String &other) {
    if (this == &other) {
        return *this;
    }

    reset();
    init(other._str);
    return *this;
}

String &String::operator=(String &&other) noexcept {
    std::swap(_str, other._str);
    std::swap(_allocated, other._allocated);
    std::swap(_bsize, other._bsize);
    std::swap(_len, other._len);

    return *this;
}

String &String::operator =(const char *other) {
    reset();
    init(other);

    return *this;
}

String &String::operator =(const std::string &other) {
    reset();
    init(other.c_str());

    return *this;
}

String &String::operator=(char32 other) {
    reset();
    *this = stringFrom(other);

    return *this;
}

String String::operator +(const String &other) {
    String s(*this);
    s.append(other);

    return s;
}

String String::operator +(const char *other) {
    String s(*this);
    s.append(other);

    return s;
}

String String::operator+=(const String &other) {
    append(other);

    return *this;
}

String String::operator+=(char32 cp) {
    append(cp);

    return *this;
}

String String::operator+=(const char *other) {
    append(other);

    return *this;
}

String String::operator+=(const std::string &other) {
    append(other);

    return *this;
}

char32 String::operator[](int64 index) const {
    return codepoint(index);
}

/*
char32 &String::operator[](size_t index) {
    return codepoint(index);
}
*/

String operator+(char *lhs, const String &rhs) {
    String s;
    s += lhs;
    s += rhs;

    return s;
}

String operator+(const std::string &lhs, const String &rhs) {
    String s;
    s += lhs.c_str();
    s += rhs;

    return s;
}

bool operator==(const String &lhs, const String &rhs) {
    if (lhs._len != rhs._len) {
        return false;
    }

    return (strcmpv(lhs._str, rhs._str) == 0);
}

bool operator!=(const String &lhs, const String &rhs) {
    return !(lhs == rhs);
}

bool operator==(const String &lhs, const char *rhs) {
    return (strcmpv(lhs._str, rhs) == 0);
}

bool operator!=(const String &lhs, const char *rhs) {
    return !(lhs == rhs);
}

std::ostream &operator <<(std::ostream &os, String &str) {
    if (!str.empty()) {
        os << str._str;
    }

    return os;
}

std::ostream &operator <<(std::ostream &os, const String &str) {
    if (!str.empty()) {
        os << str._str;
    }

    return os;
}

bool operator <(const String &lhs, const String &rhs) {
    return lhs.stdString() < rhs.stdString();
}

bool operator >(const String &lhs, const String &rhs) {
    return lhs.stdString() > rhs.stdString();
}
