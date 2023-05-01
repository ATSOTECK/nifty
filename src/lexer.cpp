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

#include "lexer.hpp"

#include <utility>
#include <iostream>

#define cpstr(x) String::stringFrom(x)

Lexer::Lexer(String path):
    _idx(0),
    _path(std::move(path)),
    _code(String::readFromFile(_path)),
    _pos(0),
    _linePos(0),
    _line(1),
    _currentChar(_code[0])
{
    _filename = _path.stringAfterLast('/');
    genTokens();
}

Token Lexer::nextToken() {
    Token t = _tokens[_idx];
    if (++_idx == _tokens.size()) {
        --_idx;
    }

    return t;
}

int Lexer::line() const {
    return _line;
}

int Lexer::linePos() const {
    return _linePos;
}

String Lexer::filename() const {
    return _filename;
}

String Lexer::path() const {
    return _path;
}

Token Lexer::next() {
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

        if (String::isNumber(_currentChar)) {
            String word;
            while ((_currentChar == '_' || _currentChar == '.' || String::isHexNumber(_currentChar) || _currentChar == 'x' || _currentChar == 'X') ||
                   _currentChar == 'o' || _currentChar == 'O' || _currentChar == 'b' || _currentChar == 'B' ||
                   (!String::isWhitespace(_currentChar) && (String::isNumber(_currentChar) && !(String::isAlpha(_currentChar))))) {
                if (_currentChar != '_') {
                    word += _currentChar;
                }
                eat();
            }

            t.type = TK_NUMBER;
            t.lexeme = word;

            return t;
        }

        if (String::isWhitespace(_currentChar)) {
            eatWhiteSpace();
            continue;
        }

        switch (_currentChar) {
            case '"': {
                String str;
                eat();
                bool overwrite = false;

                while (_currentChar != '"' || overwrite) {
                    overwrite = false;

                    if (_currentChar == '\\' && _code[_pos + 1] == '"') {
                        overwrite = true;
                    }

                    if (String::isNewline(_currentChar)) {
                        ++_line;
                        str += "\\n";
                        eat();
                        continue;
                    }

                    str += _currentChar;
                    eat();
                }
                t.type = TK_STRING_LIT;
                t.lexeme = str;
                eat();
                return t;
            }
            case '\'': {
                eat();
                t.lexeme = cpstr(_currentChar);
                eat(2); //TODO: Error if no '
                t.type = TK_CHAR_LIT;
                return t;
            }
            case ':': {
                if (peek() == ':') {
                    if (peek(2) == '=') {
                        t.lexeme = TK_VAL_DECL;
                        t.lexeme = "::=";
                        eat(3);
                        return t;
                    }

                    t.type = TK_SCOPE;
                    t.lexeme = "::";
                    eat(2);
                    return t;
                } else {
                    if (peek() == '=') {
                        t.type = TK_LET_DECL;
                        t.lexeme = ":=";
                        eat(2);
                        return t;
                    }

                    t.type = TK_COLON;
                    t.lexeme = cpstr(_currentChar);
                    eat();
                    return t;
                }
            }
            case ',': {
                t.type = TK_COMMA;
                t.lexeme = cpstr(_currentChar);
                eat();
                return t;
            }
            case 0x0000037E: //Greek Question Mark
                warn("Encountered unicode character U+037E, treating it as a semicolon.");
            case ';': {
                t.type = TK_SEMICOLON;
                t.lexeme = ';';
                eat();
                return t;
            }
            case '(': {
                t.type = TK_LPAREN;
                t.lexeme = cpstr(_currentChar);
                eat();
                return t;
            }
            case ')': {
                t.type = TK_RPAREN;
                t.lexeme = cpstr(_currentChar);
                eat();
                return t;
            }
            case '{': {
                t.type = TK_LBRACE;
                t.lexeme = cpstr(_currentChar);
                eat();
                return t;
            }
            case '}': {
                t.type = TK_RBRACE;
                t.lexeme = cpstr(_currentChar);
                eat();
                return t;
            }
            case '[': {
                t.type = TK_LBRACKET;
                t.lexeme = cpstr(_currentChar);
                eat();
                return t;
            }
            case ']': {
                t.type = TK_RBRACKET;
                t.lexeme = cpstr(_currentChar);
                eat();
                return t;
            }
            case '.': {
                if (peek() == '.') {
                    if (peek(2) == '<') {
                        t.type = TK_O_RANGE;
                        t.lexeme = "..<";
                        eat(3);
                        return t;
                    } else if (peek(2) == '=') {
                        t.type = TK_C_RANGE;
                        t.lexeme = "..=";
                        eat(2);
                        return t;
                    } else {
                        t.type = TK_VARY;
                        t.lexeme = "..";
                        eat(3);
                        return t;
                    }
                } else {
                    t.type = TK_DOT;
                    t.lexeme = cpstr(_currentChar);
                    eat();
                    return t;
                }
            }
            case '#': {
                t.type = TK_MACRO;
                eat();

                while (!String::isWhitespace(_currentChar) && String::isAlphanumeric(_currentChar)) {
                    t.lexeme += _currentChar;
                    eat();
                }

                return t;
            }
            case '=': {
                if (peek() == '=') {
                    t.type = TK_EQU;
                    t.lexeme = "==";
                    eat(2);
                    return t;
                } else {
                    t.type = TK_ASSIGN;
                    t.lexeme = cpstr(_currentChar);
                    eat();
                    return t;
                }
            }
            case '!': {
                if (peek() == '=') {
                    t.type = TK_NOTEQU;
                    t.lexeme = "!=";
                    eat(2);
                    return t;
                } else {
                    t.type = TK_NOT;
                    t.lexeme = "!";
                    eat();
                    return t;
                }
            }
            case '+': {
                if (peek() == '=') {
                    t.type = TK_ADDEQU;
                    t.lexeme = "+=";
                    eat(2);
                    return t;
                } else if (peek() == '+') {
                    t.type = TK_INC;
                    t.lexeme = "++";
                    eat(2);
                    return t;
                } else {
                    t.type = TK_ADD;
                    t.lexeme = cpstr(_currentChar);
                    eat();
                    return t;
                }
            }
            case '-': {
                if (peek() == '=') {
                    t.type = TK_SUBEQU;
                    t.lexeme = "-=";
                    eat(2);
                    return t;
                } else if (peek() == '-') {
                    t.type = TK_DEC;
                    t.lexeme = "--";
                    eat(2);
                    return t;
                } else if (peek() == '>') {
                    t.type = TK_POINT;
                    t.lexeme = "->";
                    eat(2);
                    return t;
                } else if (String::isNumber(peek())) {
                    eat();
                    String word = "-";
                    while ((_currentChar == '_' || _currentChar == '.' || String::isHexNumber(_currentChar) || _currentChar == 'x' || _currentChar == 'X') ||
                           _currentChar == 'q' || _currentChar == 'Q' || _currentChar == 'b' || _currentChar == 'B' ||
                           (!String::isWhitespace(_currentChar) && (String::isNumber(_currentChar) && !(String::isAlpha(_currentChar))))) {
                        if (_currentChar != '_') {
                            word += _currentChar;
                        }
                        eat();
                    }

                    t.type = TK_NUMBER;
                    t.lexeme = word;

                    return t;
                } else {
                    t.type = TK_SUB;
                    t.lexeme = cpstr(_currentChar);
                    eat();
                    return t;
                }
            }
            case '*': {
                if (peek() == '=') {
                    t.type = TK_MULEQU;
                    t.lexeme = "*=";
                    eat(2);
                    return t;
                } else {
                    t.type = TK_MUL;
                    t.lexeme = cpstr(_currentChar);
                    eat();
                    return t;
                }
            }
            case '/': {
                if (peek() == '/') {
                    eatLine();
                } else if (peek() == '-' || peek() == '*') {
                    eatBlock();
                } else if (peek() == '=') {
                    t.type = TK_DIVEQU;
                    t.lexeme = "/=";
                    eat(2);
                    return t;
                } else {
                    t.type = TK_DIV;
                    t.lexeme = cpstr(_currentChar);
                    eat();
                    return t;
                }
            } break;
            case '%': {
                if (peek() == '=') {
                    t.type = TK_MODEQU;
                    t.lexeme = "%=";
                    eat(2);
                    return t;
                } else if (peek() == '%') {
                    if (peek(2) == '=') {
                        t.type = TK_MODMODEQU;
                        t.lexeme = "%%=",
                        eat(3);
                        return t;
                    }

                    t.type = TK_MODMOD;
                    t.lexeme = "%%";
                    eat(2);
                    return t;
                } else if (peek() == '+') {
                    if (peek(2) == '=') {
                        t.type = TK_ADDEQU_W;
                        t.lexeme = "%+=";
                        eat(3);
                        return t;
                    } else if (peek(2) == '+') {
                        t.type = TK_INC_W;
                        t.lexeme = "%++";
                        eat(3);
                        return t;
                    }

                    t.type = TK_ADD_W;
                    t.lexeme = "%+";
                    eat(2);
                    return t;
                } else if (peek() == '-') {
                    if (peek(2) == '=') {
                        t.type = TK_SUBEQU_W;
                        t.lexeme = "%-=";
                        eat(3);
                        return t;
                    } else if (peek(2) == '-') {
                        t.type = TK_DEC_W;
                        t.lexeme = "%--";
                        eat(3);
                        return t;
                    }

                    t.type = TK_SUB_W;
                    t.lexeme = "%-";
                    eat(2);
                    return t;
                } else if (peek() == '*') {
                    if (peek(2) == '=') {
                        t.type = TK_MULEQU_W;
                        t.lexeme = "%*=";
                        eat(3);
                        return t;
                    }

                    t.type = TK_MUL_W;
                    t.lexeme = "%*";
                    eat(2);
                    return t;
                } else if (peek() == '/') {
                    if (peek(2) == '=') {
                        t.type = TK_DIVEQU_W;
                        t.lexeme = "%/=";
                        eat(3);
                        return t;
                    }

                    t.type = TK_DIV_W;
                    t.lexeme = "%/";
                    eat(2);
                    return t;
                }
                else {
                    t.type = TK_MOD;
                    t.lexeme = cpstr(_currentChar);
                    eat();
                    return t;
                }
            }
            case '^': {
                if (peek() == '=') {
                    t.type = TK_XOREQU;
                    t.lexeme = "^=";
                    eat(2);
                    return t;
                } else {
                    t.type = TK_CARET;
                    t.lexeme = cpstr(_currentChar);
                    eat();
                    return t;
                }
            }
            case '~': {
                t.type = TK_BITNOT;
                t.lexeme = cpstr(_currentChar);
                eat();
                return t;
            }
            case '>': {
                if (peek() == '=') {
                    t.type = TK_GREATEREQU;
                    t.lexeme = ">=";
                    eat(2);
                    return t;
                } if (peek() == '>') {
                    if (peek(2) == '=') {
                        t.type = TK_LSREQU;
                        t.lexeme = ">>=";
                        eat(3);
                        return t;
                    }

                    t.type = TK_LSR;
                    t.lexeme = ">>";
                    eat(2);
                    return t;
                } else {
                    t.type = TK_GREATER;
                    t.lexeme = ">";
                    eat();
                    return t;
                }
            }
            case '<': {
                if (peek() == '=') {
                    t.type = TK_LESSEQU;
                    t.lexeme = "<=";
                    eat(2);
                    return t;
                } if (peek() == '<') {
                    if (peek(2) == '=') {
                        t.type = TK_LSLEQU;
                        t.lexeme = "<<=";
                        eat(3);
                        return t;
                    }

                    t.type = TK_LSL;
                    t.lexeme = "<<";
                    eat(2);
                    return t;
                } else {
                    t.type = TK_LESS;
                    t.lexeme = "<";
                    eat();
                    return t;
                }
            }
            case '&': {
                if (peek() == '&') {
                    t.type = TK_AND;
                    t.lexeme = "&&";
                    eat(2);
                    return t;
                } else if (peek() == '=') {
                    t.type = TK_BITANDEQU;
                    t.lexeme = "&=";
                    eat(2);
                    return t;
                } else {
                    t.type = TK_BITAND;
                    t.lexeme = "&";
                    eat();
                    return t;
                }
            }
            case '|': {
                if (peek() == '|') {
                    if (peek(2) == '=') {
                        t.type = TK_NULLISH_COALESCE_ASSIGN;
                        t.lexeme = "||=";
                        eat(3);
                        return t;
                    }

                    t.type = TK_OR;
                    t.lexeme = "||";
                    eat(2);
                    return t;
                } else if (peek() == '=') {
                    t.type = TK_BITOREQU;
                    t.lexeme = "|=";
                    eat(2);
                    return t;
                } else {
                    t.type = TK_BITOR;
                    t.lexeme = "|";
                    eat();
                    return t;
                }
            }
            case '?': {
                if (peek() == '.') {
                    t.type = TK_QDOT;
                    t.lexeme = "?.";
                    eat(2);
                    return t;
                } else if (peek() == '?') {
                    if (peek(2) == '=') {
                        t.type = TK_NULL_COALESCE_ASSIGN;
                        t.lexeme = "\?\?=";
                        eat(3);
                        return t;
                    }

                    t.type = TK_NULL_COALESCE;
                    t.lexeme = "??";
                    eat(2);
                    return t;
                } else {
                    t.type = TK_QMRK;
                    t.lexeme = cpstr(_currentChar);
                    eat();
                    return t;
                }
            }
            case '@': {
                if (peek() == '+') {
                    if (peek(2) == '=') {
                        t.type = TK_ADDEQU_S;
                        t.lexeme = "@+=";
                        eat(3);
                        return t;
                    } else if (peek(2) == '+') {
                        t.type = TK_INC_S;
                        t.lexeme = "@++";
                        eat(3);

                        return t;
                    }

                    t.type = TK_ADD_S;
                    t.lexeme = "@+";
                    eat(2);
                    return t;
                } else if (peek() == '-') {
                    if (peek(2) == '=') {
                        t.type = TK_SUBEQU_S;
                        t.lexeme = "@-=";
                        eat(3);
                        return t;
                    } else if (peek(2) == '-') {
                        t.type = TK_DEC_S;
                        t.lexeme = "@--";
                        eat(3);
                        return t;
                    }

                    t.type = TK_SUB_S;
                    t.lexeme = "@-";
                    eat(2);
                    return t;
                } else if (peek() == '*') {
                    if (peek(2) == '=') {
                        t.type = TK_MULEQU_S;
                        t.lexeme = "@*=";
                        eat(3);
                        return t;
                    }

                    t.type = TK_MUL_S;
                    t.lexeme = "@*";
                    eat(2);
                    return t;
                } else if (peek() == '/') {
                    if (peek(2) == '=') {
                        t.type = TK_DIVEQU_S;
                        t.lexeme = "@/=";
                        eat(3);
                        return t;
                    }

                    t.type = TK_DIV_S;
                    t.lexeme = "@/";
                    eat(2);
                    return t;
                } else if (peek() == '<' && peek(2) == '<') {
                    if (peek(3) == '=') {
                        t.type = TK_LSLEQU_S;
                        t.lexeme = "@<<=";
                        eat(4);
                        return t;
                    }
                    t.lexeme = TK_LSL_S;
                    t.lexeme = "@<<";
                    eat(3);
                    return t;
                } else {
                    t.type = TK_OWNED;
                    t.lexeme = "@";
                    eat();
                    return t;
                }
            }
            default: {
                eat();
            } break;
        }
    }

    t.type = TK_EOF;
    t.lexeme = "EOF";

    return t;
}

void Lexer::genTokens() {
    Token t = next();
    for(;;) {
        _tokens.push_back(t);
        if (t.type == TK_EOF) {
            break;
        }

        t = next();
    }
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

void Lexer::eatLine() {
    while (!String::isNewline(_currentChar)) {
        eat();
    }
}

void Lexer::eatBlock() {
    bool endFound = false;
    uint8 nestedFound = 0;
    while (!endFound && _currentChar != EOF) {
        eat();

        if (String::isNewline(_currentChar)) {
            ++_line;
        }

        if (_currentChar == '/' && (peek() == '-' || peek() == '*')) {
            ++nestedFound;
        }

        if ((_currentChar == '-' || _currentChar == '*') && peek() == '/') {
            if (nestedFound > 0) {
                --nestedFound;
            } else {
                endFound = true;
            }
        }
    }

    eat(2);
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

char32 Lexer::peek(uint8 count) const {
    return _code[_pos + count];
}

void Lexer::warn(const String &warning) const {
    dbnln(_filename << ":L" << _line << ",C" << _linePos << ": ");
    setCmdColor(WarnColor);
    dbnln("Warning ");
    setCmdColor(TextColor);

    db("on line " << _line << ": " << warning);
    uint32 lineBegin, lineEnd = 0;

    for (int64 i = _pos; i >= 0; --i) {
        if (_code[i] == '\n') {
            lineBegin = i + 1;
            break;
        }
    }

    for (int64 i = _pos; i <= _code.length(); ++i) {
        if (_code[i] == '\n') {
            lineEnd = i - 1;
            break;
        }
    }

    String line;
    for (int64 i = lineBegin; i <= lineEnd; ++i) {
        line += _code[i];
    }

    db(line);
    setCmdColor(GreenColor);
    uint32 i;
    for (i = 0; i < _linePos - 1; ++i) {
        dbnln("~");
    }
    db("^");
    setCmdColor(TextColor);
}
