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

#ifndef __NIFTY_LEXER_HPP__
#define __NIFTY_LEXER_HPP__

#include "token.hpp"

#include <vector>

class Lexer {
public:
    explicit Lexer(String path);

    keep Token nextToken();
    keep int line() const;
    keep int linePos() const;
    keep String filename() const;
    keep String path() const;

private:
    keep Token next();
    void genTokens();

    void eat();
    void eat(uint8 amount);
    void eatLine();
    void eatBlock();
    void eatWhiteSpace();

    keep char32 peek(uint8 count = 1) const;

    void warn(const String &warning) const;

    std::vector<Token> _tokens;
    int _idx;
    String _path;
    String _filename;
    String _code;
    int _pos;
    int _linePos;
    int _line;
    int _currentChar;
};


#endif //__NIFTY_LEXER_HPP__
