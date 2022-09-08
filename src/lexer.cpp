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

#include "lexer.hpp"

#include <utility>

Lexer::Lexer(String path):
    _path(std::move(path)),
    _code(String::readFromFile(_path)),
    _pos(0),
    _linePos(0),
    _line(1),
    _currentChar(_code[0])
{
    //
}

Token Lexer::nextToken() {
    Token t;
    t.type = TK_UNKNOWN;
    t.line = _line;
    t.pos = _linePos;

    while (_currentChar != EOF && _pos < _code.length()) {
        if (String::isAlpha(_currentChar)) {
            String word;
            while (!String::isWhitespace(_currentChar) && (String::isAlphanumeric(_currentChar) || _currentChar == '_')) {
                word += _currentChar;
                eat();
            }

            t.line = _line;
            t.pos = _linePos;
            t.lexeme = word;

            std::map<String, Token>::iterator it;
            let idx = Token::keywords.find(word);
            t.type = idx != Token::keywords.end() ? Token::keywords.at(word) : TK_IDENT;

            return t;
        }

        if (String::isWhitespace(_currentChar)) {
            eatWhiteSpace();
            continue;
        }
    }

    return t;
}

void Lexer::eat() {
    ++_linePos;
    _currentChar = ++_pos >= _code.length() ? EOF : _code[_pos];
}

void Lexer::eat(uint8 amount) {
    _linePos += amount;
    _pos += amount;
    _currentChar = _pos >= _code.length() ? EOF : _code[_pos];
}

void Lexer::eatWhiteSpace() {
    while (String::isWhitespace(_currentChar)) {
        if (String::isNewline(_currentChar)) {
            _linePos = 0;
            ++_line;
        }

        eat();
    }
}
