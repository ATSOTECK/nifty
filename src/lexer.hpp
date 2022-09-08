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

#ifndef __NIFTY_LEXER_HPP__
#define __NIFTY_LEXER_HPP__

#include "token.hpp"

class Lexer {
public:
    Lexer(String path);

    Token nextToken();

    void eat();
    void eat(uint8 amount);
    void eatLine();
    void eatBlock();
    Token eatIdent();
    void eatWhiteSpace();

private:
    String _path;
    String _code;
    uint32 _pos;
    uint32 _linePos;
    uint32 _line;
    char32 _currentChar;
};


#endif //__NIFTY_LEXER_HPP__
