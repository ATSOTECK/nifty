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
    lexer->source = (string)malloc(length);
    if (lexer->source == nullptr) {
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
    if (lexer->current - lexer->start == start + len && str_eq_len(lexer->start + start, rest, len)) {
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

static NiftyTokenType checkUInts(Lexer *lexer) {
    if (str_eq_len(lexer->start + 1, "32", 2)) {
        return TK_U32;
    }
    if (str_eq_len(lexer->start + 1, "8", 1)) {
        return TK_U8;
    }
    if (str_eq_len(lexer->start + 1, "int", 3)) {
        return TK_UINT;
    }
    if (str_eq_len(lexer->start + 1, "64", 2)) {
        return TK_U64;
    }
    if (str_eq_len(lexer->start + 1, "16", 2)) {
        return TK_U16;
    }
    if (str_eq_len(lexer->start + 1, "intptr", 6)) {
        return TK_UINTPTR;
    }
    if (str_eq_len(lexer->start + 1, "128", 3)) {
        return TK_U128;
    }

    return TK_IDENT;
}

static NiftyTokenType checkSInts(Lexer *lexer) {
    if (str_eq_len(lexer->start + 1, "32", 2)) {
        return TK_S32;
    }
    if (str_eq_len(lexer->start + 1, "64", 2)) {
        return TK_S64;
    }
    if (str_eq_len(lexer->start + 1, "8", 1)) {
        return TK_S8;
    }
    if (str_eq_len(lexer->start + 1, "16", 2)) {
        return TK_S16;
    }
    if (str_eq_len(lexer->start + 1, "128", 3)) {
        return TK_S128;
    }

    return TK_IDENT;
}

static NiftyTokenType checkBoolTypes(Lexer *lexer) {
    if (str_eq_len(lexer->start + 1, "ool", 3)) {
        return TK_BOOL;
    }
    if (str_eq_len(lexer->start + 1, "8", 1)) {
        return TK_B8;
    }
    if (str_eq_len(lexer->start + 1, "16", 2)) {
        return TK_B16;
    }
    if (str_eq_len(lexer->start + 1, "32", 2)) {
        return TK_B32;
    }
    if (str_eq_len(lexer->start + 1, "64", 2)) {
        return TK_B64;
    }

    return TK_IDENT;
}

static NiftyTokenType checkFloatTypes(Lexer *lexer) {
    if (str_eq_len(lexer->start + 1, "32", 2)) {
        return TK_F32;
    }
    if (str_eq_len(lexer->start + 1, "oat", 3)) {
        return TK_FLOAT;
    }
    if (str_eq_len(lexer->start + 1, "64", 2)) {
        return TK_F64;
    }
    if (str_eq_len(lexer->start + 1, "16", 2)) {
        return TK_F16;
    }
    if (str_eq_len(lexer->start + 1, "128", 3)) {
        return TK_F128;
    }

    return TK_IDENT;
}

static NiftyTokenType identType(Lexer *lexer) {
    switch (lexer->start[0]) {
        case '_': return checkKeyword(lexer, 1, 8, "_anytype", TK_ANY_TYPE); // __anytype
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
        case 'b': {
            // behavior
            // break
            // bool b8 b16 b32 b64
            NiftyTokenType type = checkKeyword2(lexer, 1, 4, "reak", TK_BREAK, 7, "ehavior", TK_BEHAVIOR);
            if (type != TK_IDENT) {
                return type;
            }
            return checkBoolTypes(lexer);
        }
        case 'c':
            // cast
            // char
            // const
            // constimpl
            // continue
            // cstring
            if (lexer->current - lexer->start > 1) {
                switch (lexer->start[1]) {
                    case 'a': return checkKeyword(lexer, 2, 2, "st", TK_CAST);
                    case 'h': return checkKeyword(lexer, 2, 2, "ar", TK_CHAR_TYPE);
                    case 'o':
                        if (lexer->start[2] == 'n' && lexer->start[3] == 's') {
                            return checkKeyword2(lexer, 4, 1, "t", TK_CONST, 5, "timpl", TK_CONST_IMPL);
                        } else {
                            return checkKeyword(lexer, 3, 5, "ntinue", TK_CONTINUE);
                        }
                    case 's': return checkKeyword(lexer, 2, 5, "tring", TK_CSTRING_TYPE);
                }
            }
        case 'd':
            // defer
            // defer_err
            // delete
            // does
            // double
            if (lexer->current - lexer->start > 1) {
                switch (lexer->start[1]) {
                    case 'e':
                        if (lexer->start[2] == 'f') {
                            return checkKeyword2(lexer, 3, 2, "er", TK_DEFER, 6, "er_err", TK_DEFER_ERR);
                        } else {
                            return checkKeyword(lexer, 2, 4, "lete", TK_DELETE);
                        }
                    case 'o': return checkKeyword2(lexer, 2, 2, "es", TK_DOES, 4, "uble", TK_DOUBLE);
                }
            }
        case 'e':
            // else
            // elif
            // emit
            // endimpl
            // enum
            // extern
            if (lexer->current - lexer->start > 1) {
                switch (lexer->start[1]) {
                    case 'l': return checkKeyword2(lexer, 2, 2, "se", TK_ELSE, 2, "if", TK_ELIF);
                    case 'm': return checkKeyword(lexer, 2, 2, "it", TK_EMIT);
                    case 'n': return checkKeyword2(lexer, 2, 2, "um", TK_ENUM, 3, "dimpl", TK_END_IMPL);
                    case 'x': return checkKeyword(lexer, 2, 4, "tern", TK_EXTERN);
                }
            }
        case 'f': {
            // false
            // float
            // f16 f32 f64
            // fn
            // for
            NiftyTokenType type = checkKeyword(lexer, 1, 1, "n", TK_FN);
            if (type != TK_IDENT) {
                return type;
            }
            type = checkFloatTypes(lexer);
            if (type != TK_IDENT) {
                return type;
            }
            return checkKeyword2(lexer, 1, 2, "or", TK_FOR, 4, "alse", TK_FALSE);
        }
        case 'g': return checkKeyword(lexer, 1, 3, "oto", TK_GOTO); // goto
        case 'h': break;
        case 'i':
            // if
            // impl
            // in
            // int
            if (lexer->current - lexer->start > 1) {
                switch (lexer->start[1]) {
                    case 'f': return checkKeyword(lexer, 2, 0, "", TK_IF);
                    case 'm': return checkKeyword(lexer, 2, 2, "pl", TK_IMPL);
                    case 'n': return checkKeyword2(lexer, 2, 0, "", TK_IN, 1, "t", TK_INT);
                }
            }
        case 'j': break;
        case 'k': break;
        case 'l': return checkKeyword(lexer, 1, 2, "et", TK_LET); // let
        case 'm': return checkKeyword(lexer, 1, 1, "d", TK_MD); // md
        case 'n':
            // name_of
            // new
            // null
            if (lexer->current - lexer->start > 1) {
                switch (lexer->start[1]) {
                    case 'a': return checkKeyword(lexer, 2, 5, "me_of", TK_NAME_OF);
                    case 'e': return checkKeyword(lexer, 2, 1, "w", TK_NEW);
                    case 'u': return checkKeyword(lexer, 2, 2, "ll", TK_NULL);
                }
            }
        case 'o': break;
        case 'p': return checkKeyword(lexer, 1, 6, "ackage", TK_PACKAGE); // package
        case 'q': break;
        case 'r':
            // rawptr
            // recast
            // restrict
            // return
            if (lexer->current - lexer->start > 1) {
                switch (lexer->start[1]) {
                    case 'a': return checkKeyword(lexer, 2, 3, "ptr", TK_RAWPTR);
                    case 'e':
                        switch (lexer->start[2]) {
                            case 'c': return checkKeyword(lexer, 3, 3, "ast", TK_RECAST);
                            case 's': return checkKeyword(lexer, 3, 6, "strict", TK_RESTRICT);
                            case 't': return checkKeyword(lexer, 3, 3, "urn", TK_RETURN);
                        }
                }
            }
        case 's': {
            // size_of
            // skip
            // string
            // struct
            // s8 s16 s32 s64 s128
            NiftyTokenType type = checkSInts(lexer);
            if (type != TK_IDENT) {
                return type;
            }
            if (lexer->current - lexer->start > 1) {
                switch (lexer->start[1]) {
                    case 'i': return checkKeyword(lexer, 2, 5, "ze_of", TK_SIZE_OF);
                    case 'k': return checkKeyword(lexer, 2, 2, "ip", TK_SKIP);
                    case 't': return checkKeyword2(lexer, 2, 4, "ring", TK_STRING_TYPE, 4, "ruct", TK_STRUCT);
                }
            }
        }
        case 't':
            // test
            // true
            // try
            // type
            // type_of
            // type_from
            // typeid_of
            // typeinfo_of
            // typeid
            if (lexer->current - lexer->start > 1) {
                switch (lexer->start[1]) {
                    case 'e': return checkKeyword(lexer, 2, 2, "st", TK_TEST);
                    case 'r': return checkKeyword2(lexer, 2, 1, "y", TK_TRY, 2, "ue", TK_TRUE);
                    case 'y': {
                        NiftyTokenType type = checkKeyword(lexer, 2, 2, "pe", TK_TYPE);
                        if (type != TK_IDENT) {
                            return type;
                        }
                        type = checkKeyword2(lexer, 2, 5, "pe_of", TK_TYPE_OF, 7, "pe_from", TK_TYPE_FROM);
                        if (type != TK_IDENT) {
                            return type;
                        }
                        type = checkKeyword2(lexer, 2, 7, "peid_of", TK_TYPEID_OF, 9, "peinfo_of", TK_TYPEINFO_OF);
                        if (type != TK_IDENT) {
                            return type;
                        }
                        return checkKeyword(lexer, 2, 4, "peid", TK_TYPEID);
                    }
                }
            }
        case 'u': {
            // until
            // use
            // using
            // u8 u16 u32 u64 u128
            // uint
            // uintptr
            // unused
            // undefined
            NiftyTokenType type = checkKeyword2(lexer, 1, 2, "se", TK_USE, 4, "sing", TK_USING);
            if (type != TK_IDENT) {
                return type;
            }
            type = checkKeyword(lexer, 1, 4, "ntil", TK_UNTIL);
            if (type != TK_IDENT) {
                return type;
            }
            type = checkUInts(lexer);
            if (type != TK_IDENT) {
                return type;
            }
            return checkKeyword2(lexer, 1, 8, "ndefined", TK_UNDEFINED, 5, "nused", TK_UNUSED);
        }
        case 'v':
            // val
            // void
            if (lexer->current - lexer->start > 1) {
                switch (lexer->start[1]) {
                    case 'a': return checkKeyword(lexer, 2, 1, "l", TK_VAL);
                    case 'o': return checkKeyword(lexer, 2, 2, "id", TK_VOID);
                }
            }
        case 'w': return checkKeyword2(lexer, 1, 4, "hile", TK_WHILE, 2, "en", TK_WHEN); // when, while
        case 'x': break;
        case 'y': break;
        case 'z': break;
    }

    return TK_IDENT;
}

static Token stringLit(Lexer *lexer, char strChar) {
    //TODO: Raw strings.

    bool overwrite = false;
    while ((peek(lexer) != strChar || overwrite) && !atEnd(lexer)) {
        overwrite = false;

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
        case ':': {
            if (match(lexer, '=')) {
                return makeToken(lexer, TK_LET_DECL);
            } else if (match(lexer, ':')) {
                return makeToken(lexer, match(lexer, '=') ? TK_CONST_DECL : TK_SCOPE);
            } else {
                return makeToken(lexer, TK_COLON);
            }
        }
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

void printToken(Token token) {
    if (token.type == TK_INTERNAL_ERROR) {
        println("TK_INTERNAL_ERROR");
        return;
    }

    println("Token '%.*s'", token.len, token.lexeme);
}
