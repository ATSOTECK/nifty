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

#ifndef __NIFTY_TOKEN_HPP__
#define __NIFTY_TOKEN_HPP__

#include <map>

#include "util/str.hpp"

enum Tokens {
    TK_UNKNOWN = 0,

    TK_FN,
    TK_MD,

    TK_IF,
    TK_IF_NOT,
    TK_ELIF,
    TK_ELSE,
    TK_DO,
    TK_IN,
    TK_AS,
    TK_CAST,
    TK_RECAST,
    TK_AUTOCAST,
    TK_STRUCT,
    TK_IMPL,
    TK_CONSTIMPL,
    TK_END_IMPL,
    TK_DOES,
    TK_BEHAVIOR,
    TK_ENUM,
    TK_INTERFACE, // Remove?

    TK_DEFER,
    TK_DEFER_ERR,
    TK_RESTRICT,

    TK_TYPE,
    TK_FOR,
    TK_WHILE,
    TK_UNTIL,
    TK_WHEN,
    TK_GOTO,
    TK_BREAK,
    TK_CONTINUE,
    TK_RETURN,
    TK_TRY,

    TK_TRUE,
    TK_FALSE,

    TK_LET, // mutable
    TK_VAL, // immutable
    TK_CONST, // immutable, compile time
    TK_NEW,
    TK_DELETE,
    TK_THIS,

    TK_USE,
    TK_USING,
    TK_PACKAGE,
    TK_EXTERN,

    TK_SIZE_OF,
    TK_ALIGN_OF,
    TK_TYPE_OF,
    TK_TYPEID_OF,
    TK_TYPEINFO_OF,
    TK_TYPE_FROM,
    TK_NAME_OF,

    TK_INT,
    TK_UINT,
    TK_FLOAT,
    TK_DOUBLE,
    TK_STRING_TYPE,
    TK_CSTRING_TYPE,
    TK_CHAR_TYPE,
    TK_BOOL,
    TK_B8,
    TK_B16,
    TK_B32,
    TK_B64,
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
    TK_F16,
    TK_F32,
    TK_F64,
    TK_F128,
    TK_VOID,
    TK_RAWPTR,
    TK_TYPEID,

    TK_NULL,
    TK_UNDEFINED,
    TK_UNUSED,

    TK_EMIT,

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
    TK_C_RANGE,                 // ..= [a, b]
    TK_O_RANGE,                 // ..< [a, b)
    TK_VARY,                    // ..
    TK_LABEL,                   // ident:
    TK_OWNED,                   // @
    TK_MACRO,                   // #
    TK_SCOPE,                   // ::
    TK_LET_DECL,                // :=
    TK_VAL_DECL,                // ::=    Also used for const.

    TK_ASSIGN,                  // =
    TK_EQU,                     // == is
    TK_AND,						// && and
    TK_OR,						// || or
    TK_NOT,						// ! not
    TK_NOTEQU,					// !=
    TK_ADD,                     // +
    TK_INC,                     // x++
    TK_ADDEQU,                  // +=
    TK_SUB,                     // -
    TK_DEC,                     // x--
    TK_SUBEQU,                  // -=
    TK_MUL,                     // *
    TK_MULEQU,                  // *=
    TK_DIV,                     // /
    TK_DIVEQU,                  // /=
    TK_MOD,                     // %
    TK_MODEQU,                  // %=
    TK_MODMOD,                  // %%
    TK_MODMODEQU,               // %%=

    TK_ADD_W,                   // %+
    TK_SUB_W,                   // %-
    TK_MUL_W,                   // %*
    TK_DIV_W,                   // %/
    TK_ADDEQU_W,                // %+=
    TK_SUBEQU_W,                // %-=
    TK_MULEQU_W,                // %*=
    TK_DIVEQU_W,                // %/=
    TK_INC_W,                   // %++
    TK_DEC_W,                   // %--

    TK_ADD_S,                   // @+
    TK_SUB_S,                   // @-
    TK_MUL_S,                   // @*
    TK_DIV_S,                   // @/
    TK_ADDEQU_S,                // @+=
    TK_SUBEQU_S,                // @-=
    TK_MULEQU_S,                // @*=
    TK_DIVEQU_S,                // @/=
    TK_INC_S,                   // @++
    TK_DEC_S,                   // @--
    TK_LSL_S,                   // @<<
    TK_LSLEQU_S,                // @<<=

    TK_GREATER,					// >
    TK_GREATEREQU,				// >=
    TK_LESS,					// <
    TK_LESSEQU,					// <=

    TK_XOR,                     // ^
    TK_BITNOT,                  // ~
    TK_BITAND,                  // &
    TK_BITOR,                   // |
    TK_LSL,                     // <<
    TK_LSR,                     // >>
    TK_XOREQU,                  // ^=
    TK_BITANDEQU,               // &=
    TK_BITOREQU,                // |=
    TK_LSLEQU,                  // <<=
    TK_LSREQU,                  // >>=

    TK_POINT,                   // ->

    TK_QMRK,                    // ?
    TK_QDOT,                    // ?.
    TK_NULL_COALESCE,           // ??
    TK_NULL_COALESCE_ASSIGN,    // ??=
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
    int line;
    int pos;
    int index;

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
