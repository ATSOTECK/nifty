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

#include "lexer.h"

#include <stdlib.h>
#include <string.h>

Lexer *initLexer(conststr entryPoint) {
    Lexer *lexer = (Lexer*)malloc(sizeof(Lexer));

    FILE *file = fopen(entryPoint, "r");
    if (file == nullptr) {
        println("Could not open '%s' for reading.", entryPoint);
        return nullptr;
    }

    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);
    lexer->start = (string)malloc(length);
    if (lexer->start == nullptr) {
        println("Out of memory.");
        return nullptr;
    }

    fread(lexer->source, 1, length, file);
    fclose(file);

    lexer->start = lexer->source;
    lexer->current = lexer->start;
    lexer->prev = '\0';
    lexer->line = 1;

    return lexer;
}

void freeLexer(Lexer *lexer) {
    free(lexer->source);
    free(lexer);
}

static inline bool atEnd(Lexer *lexer) {
    return *lexer->current == '\0';
}

static Token makeToken(Lexer *lexer, NiftyTokenType type) {
    Token token;
    token.type = type;
    token.lexeme = lexer->start;
    token.len = (int)(lexer->current - lexer->start);
    token.line = lexer->line;

    return token;
}

static Token errorToken(Lexer *lexer, conststr msg) {
    Token token;
    token.type = TK_INTERNAL_ERROR;
    token.lexeme = msg;
    token.len = (int)str_len(msg);
    token.line = lexer->line;

    return token;
}

static char advance(Lexer *lexer) {
    lexer->prev = *lexer->current;
    lexer->current++;
    return lexer->current[-1];
}

inline static char peek(Lexer *lexer) {
    return *lexer->current;
}

inline static char peekNext(Lexer *lexer) {
    if (atEnd(lexer)) {
        return '\0';
    }

    return lexer->current[1];
}

static void skipWhitespace(Lexer *lexer) {
    for (;;) {
        char c = peek(lexer);
        switch (c) {
            case ' ':
            case '\r':
            case '\t':
                advance(lexer);
                break;
            case '\n':
                ++lexer->line;
                advance(lexer);
                break;
            case '/':
                if (peekNext(lexer) == '/') {
                    while (peek(lexer) != '\n' && !atEnd(lexer)) {
                        advance(lexer);
                    }
                } else if (peekNext(lexer) == '-' || peekNext(lexer) == '*') {
                    int level = 0;
                    advance(lexer);
                    while (!atEnd(lexer) && (level >= 1 || !((peek(lexer) == '-' || peek(lexer) == '*') && peekNext(lexer) == '/'))) {
                        if (peek(lexer) == '\n') {
                            lexer->line++;
                        } else if (peek(lexer) == '/' && (peekNext(lexer) == '-' || peekNext(lexer) == '*')) {
                            ++level;
                        } else if ((peek(lexer) == '-' || peek(lexer) == '*') && peekNext(lexer) == '/') {
                            --level;
                        }

                        advance(lexer);
                    }

                    advance(lexer); // - or *
                    advance(lexer);// /
                } else {
                    return;
                }
                break;
            default:
                return;
        }
    }
}

static NiftyTokenType checkKeyword(Lexer *lexer, int start, int len, conststr rest, NiftyTokenType type) {
    if (lexer->current - lexer->start == start + len && memcmp(lexer->start + start, rest, len) == 0) {
        return type;
    }

    return TK_IDENT;
}

static NiftyTokenType checkKeyword2(Lexer *lexer, int start, int len, conststr rest, NiftyTokenType type, int len2, conststr rest2, NiftyTokenType type2) {
    NiftyTokenType tokenType = checkKeyword(lexer, start, len, rest, type);
    if (tokenType == TK_IDENT) {
        return checkKeyword(lexer, start, len2, rest2, type2);
    }

    return tokenType;
}

static bool match(Lexer *lexer, char expected) {
    if (atEnd(lexer)) {
        return false;
    }

    if (*lexer->current != expected) {
        return false;
    }
    lexer->current++;

    return true;
}

static NiftyTokenType identType(Lexer *lexer) {
    switch (lexer->start[0]) {
        case 'a':
            // align_of
            // api
            // as
            // assert
            // assert_db
            // auto_cast
            if (lexer->current - lexer->start > 1) {
                switch (lexer->start[1]) {
                    case 'l': return checkKeyword(lexer, 2, 6, "ign_of", TK_ALIGN_OF);
                    case 'p': return checkKeyword(lexer, 2, 1, "i", TK_API);
                    case 's':
                        if (lexer->start[2] == 's') {
                            return checkKeyword2(lexer, 2, 4, "sert", TK_ASSERT, 7, "sert_db", TK_ASSERT_DB);
                        } else {
                            return checkKeyword(lexer, 2, 0, "", TK_AS);
                        }
                    case 'u': return checkKeyword(lexer, 2, 7, "to_cast", TK_AUTOCAST);
                }
            }
    }

    return TK_IDENT;
}

static Token stringLit(Lexer *lexer, char strChar) {
    //TODO: Raw strings.

    bool overwrite = false;
    bool skipInStr = false;
    while ((peek(lexer) != strChar || overwrite) && !atEnd(lexer)) {
        overwrite = false;
        skipInStr = false;

        if (peek(lexer) == '\\' && peekNext(lexer) == strChar) {
            overwrite = true;
        }

        if (peek(lexer) == '\n') {
            lexer->line++;
        }

        advance(lexer);
    }

    if (atEnd(lexer)) {
        return errorToken(lexer, "Unterminated string.");
    }

    advance(lexer);
    return makeToken(lexer, strChar == '"' ? TK_STRING_LIT : TK_CHAR_LIT);
}

static Token ident(Lexer *lexer) {
    while (isAlphaNumeric(peek(lexer))) {
        advance(lexer);
    }

    return makeToken(lexer, identType(lexer));
}

static Token exponent(Lexer *lexer) {
    advance(lexer);

    while (peek(lexer) == '_') {
        advance(lexer);
    }

    if (peek(lexer) == '-' || peek(lexer) == '+') {
        advance(lexer);
    }

    if (!isDigit(peek(lexer)) && peek(lexer) != '_') {
        return errorToken(lexer, "Invalid exponent literal.");
    }

    while (isDigit(peek(lexer)) || peek(lexer) == '_') {
        advance(lexer);
    }

    return makeToken(lexer, TK_NUMBER);
}

static Token number(Lexer *lexer) {
    while (isDigit(peek(lexer)) || peek(lexer) == '_') {
        advance(lexer);
    }

    if (peek(lexer) == 'e' || peek(lexer) == 'E') {
        return exponent(lexer);
    }

    if (peek(lexer) == '.' && isDigit(peekNext(lexer))) {
        advance(lexer);

        while (isDigit(peek(lexer)) || peek(lexer) == '_') {
            advance(lexer);
        }

        if (peek(lexer) == 'e' || peek(lexer) == 'E') {
            return exponent(lexer);
        }
    }

    return makeToken(lexer, TK_NUMBER);
}

static Token octalNumber(Lexer *lexer) {
    while (peek(lexer) == '_') {
        advance(lexer);
    }

    if (peek(lexer) == '0') {
        advance(lexer);
    }

    if (peek(lexer) == 'o' || peek(lexer) == 'O' || peek(lexer) == 'q' || peek(lexer) == 'Q') {
        advance(lexer);
        if (!isOctDigit(peek(lexer))) {
            return errorToken(lexer, "Invalid octal literal.");
        }

        while (isOctDigit(peek(lexer))) {
            advance(lexer);
        }

        return makeToken(lexer, TK_NUMBER);
    } else {
        return number(lexer);
    }
}

static Token hexNumber(Lexer *lexer) {
    while (peek(lexer) == '_') {
        advance(lexer);
    }

    if (peek(lexer) == '0') {
        advance(lexer);
    }

    if (peek(lexer) == 'x' || peek(lexer) == 'X') {
        advance(lexer);
        if (!isHexDigit(peek(lexer))) {
            return errorToken(lexer, "Invalid hex literal.");
        }

        while (isHexDigit(peek(lexer))) {
            advance(lexer);
        }

        return makeToken(lexer, TK_NUMBER);
    } else {
        return octalNumber(lexer);
    }
}

Token nextToken(Lexer *lexer) {
    skipWhitespace(lexer);
    lexer->start = lexer->current;

    if (atEnd(lexer)) {
        return makeToken(lexer, TK_EOF);
    }

    char c = advance(lexer);
    if (isAlpha(c)) {
        return ident(lexer);
    }

    if (isDigit(c)) {
        return hexNumber(lexer);
    }

    switch (c) {
        case '(': return makeToken(lexer, TK_LPAREN);
        case ')': return makeToken(lexer, TK_RPAREN);
        case '{': return makeToken(lexer, TK_LBRACE);
        case '}': return makeToken(lexer, TK_RBRACE);
        case '[': return makeToken(lexer, TK_LBRACKET);
        case ']': return makeToken(lexer, TK_RBRACKET);
        case ';': return makeToken(lexer, TK_SEMICOLON);
        case ',': return makeToken(lexer, TK_COMMA);
        case '.': return makeToken(lexer, TK_DOT);
        case '#': return makeToken(lexer, TK_HASH);
        case '~': return makeToken(lexer, TK_BIT_NOT);
        case '+': {
            if (match(lexer, '+')) {
                return makeToken(lexer, TK_INC);
            } else if (match(lexer, '=')) {
                return makeToken(lexer, TK_ADDEQ);
            } else {
                return makeToken(lexer, TK_ADD);
            }
        }
        case '-': {
            if (match(lexer, '-')) {
                return makeToken(lexer, TK_DEC);
            } else if (match(lexer, '=')) {
                return makeToken(lexer, TK_SUBEQ);
            } else if (match(lexer, '>')) {
                return makeToken(lexer, TK_ARROW);
            } else {
                return makeToken(lexer, TK_SUB);
            }
        }
        case '*': return makeToken(lexer, match(lexer, '=') ? TK_MULEQ : TK_MUL);
        case '/': return makeToken(lexer, match(lexer, '=') ? TK_DIVEQ : TK_DIV);
        case '%': return makeToken(lexer, match(lexer, '=') ? TK_MODEQ : TK_MOD);
        case '!': return makeToken(lexer, match(lexer, '=') ? TK_NOTEQ : TK_BANG);
        case '=': return makeToken(lexer, match(lexer, '=') ? TK_EQ    : TK_ASSIGN);
        case '<': {
            if (match(lexer, '-')) {
                return makeToken(lexer, TK_LEFT_ARROW);
            } else if (match(lexer, '<')) {
                return makeToken(lexer, TK_LSL);
            } else {
                return makeToken(lexer, match(lexer, '=') ? TK_LTEQ : TK_LT);
            }
        }
        case '>': {
            if (match(lexer, '>')) {
                return makeToken(lexer, TK_LSR);
            } else {
                return makeToken(lexer, match(lexer, '=') ? TK_GREQ : TK_GR);
            }
        }
        case '&': {
            if (match(lexer, '&')) {
                return makeToken(lexer, TK_AND);
            } else if (match(lexer, '=')) {
                return makeToken(lexer, TK_BIT_ANDEQ);
            } else {
                return makeToken(lexer, TK_BIT_AND);
            }
        }
        case '|': {
            if (match(lexer, '|')) {
                return makeToken(lexer, TK_OR);
            } else if (match(lexer, '=')) {
                return makeToken(lexer, TK_BIT_OREQ);
            } else {
                return makeToken(lexer, TK_BIT_OR);
            }
        }
        case '^': return makeToken(lexer, match(lexer, '=') ? TK_BIT_XOR_EQ : TK_CARET);
        case '"': return stringLit(lexer, '"');
        case '\'': return stringLit(lexer, '\'');
        default: break;
    }

    return errorToken(lexer, "Unexpected character.");
}
