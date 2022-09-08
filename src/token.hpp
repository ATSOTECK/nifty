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

#ifndef __NIFTY_TOKEN_HPP__
#define __NIFTY_TOKEN_HPP__

#include <map>

#include "util/str.hpp"

enum Tokens {
    TK_UNKNOWN = 0,

    TK_FN,
    TK_STATIC,

    TK_IF,
    TK_IF_NOT,
    TK_ELIF,
    TK_ELSE,
    TK_DO,
    TK_IN,
    TK_STEP,
    TK_AS,
    TK_CAST,
    TK_STRUCT,
    TK_IMPL,
    TK_DOES,
    TK_BEHAVIOR,
    TK_ENUM,
    TK_UNION,
    TK_PUBLIC,
    TK_PRIVATE,
    TK_READ_ONLY,

    TK_DELAY,
    TK_ASSERT,

    TK_TYPE,
    TK_FOR,
    TK_FOREVER,
    TK_WHILE,
    TK_UNTIL,
    TK_WHEN,
    TK_REPEAT, // could be a macro
    TK_GOTO,
    TK_BREAK,
    TK_CONTINUE,
    TK_RETURN,
    TK_DEFAULT,

    TK_ASM,
    TK_INLINE,

    TK_TRUE,
    TK_FALSE,

    TK_GLOBAL,
    TK_LOCAL,
    TK_LET, // mutable
    TK_VAL, // immutable
    TK_CONST, // immutable, compile time
    TK_NEW,
    TK_DELETE,

    TK_USE,
    TK_NAMESPACE,

    TK_SIZEOF,
    TK_ALIGNOF,
    TK_TYPEOF,
    TK_NAMEOF,

    TK_INT,
    TK_FLOAT,
    TK_DOUBLE,
    TK_STRING_TYPE,
    TK_CHAR_TYPE,
    TK_BOOL,
    TK_U8,
    TK_U16,
    TK_U32,
    TK_U64,
    TK_U128,
    TK_S8,
    TK_S16,
    TK_S32,
    TK_S64,
    TK_S128,
    TK_ISIZE,
    TK_USIZE,
    TK_VOID,
    TK_NULL,

    TK_OPERATOR,

    TK_COLON,                   // :
    TK_COMMA,                   // ,
    TK_SEMICOLON,               // ;
    TK_LPAREN,                  // (
    TK_RPAREN,                  // )
    TK_LBRACE,                  // {
    TK_RBRACE,                  // }
    TK_LBRACKET,                // [
    TK_RBRACKET,                // ]
    TK_DOT,                     // .
    TK_IRANGE,                  // ..
    TK_ERANGE,                  // ..<
    TK_VARY,                    // ...
    TK_LABEL,                   // ident:
    TK_ANNOTATION,              // @
    TK_MACRO,                   // #
    TK_SCOPE,                   // ::
    TK_LET_DECL,                // :=
    TK_VAL_DECL,                // ::=

    TK_ASSIGN,                  // =
    TK_EQU,                     // == is
    TK_AND,						// && and
    TK_OR,						// || or
    TK_NOT,						// ! not
    TK_NOTEQU,					// !=
    TK_ADD,                     // +
    TK_INC,                     // x++
    TK_PREINC,                  // ++x
    TK_ADDEQU,                  // +=
    TK_SUB,                     // -
    TK_DEC,                     // x--
    TK_PREDEC,                  // --x
    TK_SUBEQU,                  // -=
    TK_MUL,                     // *
    TK_MULEQU,                  // *=
    TK_DIV,                     // /
    TK_DIVEQU,                  // /=
    TK_MOD,                     // %
    TK_MODEQU,                  // %=

    TK_POS,                     // +x
    TK_NEG,                     // -x

    TK_GREATER,					// >
    TK_GREATEREQU,				// >=
    TK_LESS,					// <
    TK_LESSEQU,					// <=

    TK_XOR,                     // ^
    TK_BITNOT,                  // ~
    TK_BITAND,                  // &
    TK_BITOR,                   // |
    TK_BITLS,                   // <<
    TK_BITRS,                   // >>
    TK_XOREQU,                  // ^=
    TK_BITANDEQU,               // &=
    TK_BITOREQU,                // |=
    TK_BITLSEQU,                // <<=
    TK_BITRSEQU,                // >>=

    TK_ADDR,                    // &x
    TK_DEREF,                   // x^
    TK_POINT,                   // ->
    TK_PDREF,                   // ->*
    TK_DOTREF,                  // .*

    TK_QMRK,                    // ?
    TK_QDOT,                    // ?.
    TK_NULL_COALESCE,           // ??
    TK_NULL_COALESCE_ASSIGN,    // ??=
    TK_NULLISH_COALESCE,        // ||
    TK_NULLISH_COALESCE_ASSIGN, // ||=

    TK_IDENT,
    TK_STRING_LIT,
    TK_CHAR_LIT,
    TK_NUMBER,

    TK_EOF
};

struct Token {
    String lexeme;
    int type;
    uint32 line;
    uint32 pos;
    uint32 index;

    Token &operator=(const Token &other) = default;

    static std::map<String, Tokens> keywords;
};

String tokenToString(const Token &t);
bool isTokenType(int type);
bool isTokenOp(int type);
bool isTokenBinOp(int type);
bool isTokenOpPrePostFix(int type);
bool isTokenUnaryOp(int type);
int getTokenPrecedence(int type);
int getNumBitsForToken(int type);
bool isSignedForToken(int type);

#endif //__NIFTY_TOKEN_HPP__
