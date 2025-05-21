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

#ifndef __NIFTY_LEXER_H__
#define __NIFTY_LEXER_H__

typedef enum {
    TK_UNKNOWN = 0,
    
    TK_FN, // fn
    TK_MD, // md

    TK_TEST, // test
    TK_SKIP, // skip
    
    TK_IF, // if
    TK_ELIF, // elif
    TK_ELSE, // else
    TK_IN, // in
    TK_AS, // as
    TK_CAST, // case
    TK_RECAST, // recast
    TK_AUTO_CAST, // auto_cast
    TK_STRUCT, // struct
    TK_IMPL, // impl
    TK_CONST_IMPL, // constimpl
    TK_END_IMPL, // endimpl
    TK_DOES, // does
    TK_BEHAVIOR, // behavior
    TK_ENUM, // enum
    
    TK_DEFER, // defer
    TK_DEFER_ERR, // defer_err
    TK_RESTRICT, // restrict
    
    TK_TYPEDEF, // typedef
    TK_FOR, // for
    TK_WHILE, // while
    TK_UNTIL, // until
    TK_WHEN, // when
    TK_GOTO, // goto
    TK_BREAK, // break
    TK_CONTINUE, // continue
    TK_RETURN, // return
    TK_TRY, // try
    
    TK_TRUE, // true
    TK_FALSE, // false

    TK_MUT, // mutable
    TK_LET, // mutable
    TK_VAL, // immutable, run time
    TK_CONST, // immutable, compile time
    TK_NEW, // new
    TK_DELETE, // delete
    
    TK_USE, // use
    TK_USING, // using
    TK_NAMESPACE, // namespace
    TK_PACKAGE, // package
    TK_API, // api
    TK_EXTERN, // extern
    
    TK_SIZE_OF, // size_of
    TK_ALIGN_OF, // align_of
    TK_TYPE_OF, // type_of
    TK_TYPEID_OF, // typeid_of
    TK_TYPEINFO_OF, // typeinfo_of
    TK_TYPE_FROM, // type_from
    TK_NAME_OF, // name_of
    
    TK_INT, // int
    TK_UINT, // uint
    TK_FLOAT, // float
    TK_DOUBLE, // double
    TK_STRING_TYPE, // string
    TK_CSTRING_TYPE, // cstring
    TK_CHAR_TYPE, // char
    TK_BOOL, // bool
    TK_B8, // b8
    TK_B16, // b16
    TK_B32, // b32
    TK_B64, // b64
    TK_U8, // u8
    TK_U16, // u16
    TK_U32, // u32
    TK_U64, // u64
    TK_U128, // u128
    TK_S8, // s8
    TK_S16, // s16
    TK_S32, // s32
    TK_S64, // s64
    TK_S128, // s128
    TK_F16, // f16
    TK_F32, // f32
    TK_F64, // f64
    TK_F128, // f128
    TK_VOID, // void
    TK_RAWPTR, // rawptr
    TK_UINTPTR, // uintptr
    TK_TYPEID, // type_id
    TK_ANY_TYPE, // __anytype
    
    TK_NULL, // null
    TK_UNDEFINED, // undefined
    TK_UNUSED, // unsuded
    
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
    TK_DOT_DOT,                 // ..
    TK_AT,                      // @
    TK_HASH,                    // #
    TK_SCOPE,                   // ::
    TK_LET_DECL,                // :=
    TK_CONST_DECL,              // ::=    Also used for val.
    
    TK_ASSIGN,                  // =
    TK_EQ,                      // ==
    TK_AND,						// &&
    TK_OR,						// ||
    TK_BANG,				    // !
    TK_NOT_EQ, 					// !=
    TK_ADD,                     // +
    TK_INC,                     // x++
    TK_ADD_EQ,                  // +=
    TK_SUB,                     // -
    TK_DEC,                     // x--
    TK_SUB_EQ,                  // -=
    TK_MUL,                     // *
    TK_MUL_EQ,                  // *=
    TK_DIV,                     // /
    TK_DIV_EQ,                  // /=
    TK_MOD,                     // %
    TK_MOD_EQ,                  // %=

    // wrapping arithmetic
    TK_ADD_W,                   // %+
    TK_SUB_W,                   // %-
    TK_MUL_W,                   // %*
    TK_ADD_EQ_W,                // %+=
    TK_SUB_EQ_W,                // %-=
    TK_MUL_EQ_W,                // %*=
    TK_INC_W,                   // %++
    TK_DEC_W,                   // %--

    // saturating arithmetic
    TK_ADD_S,                   // @+
    TK_SUB_S,                   // @-
    TK_MUL_S,                   // @*
    TK_ADD_EQ_S,                // @+=
    TK_SUB_EQ_S,                // @-=
    TK_MUL_EQ_S,                // @*=
    TK_INC_S,                   // @++
    TK_DEC_S,                   // @--
    TK_LSL_S,                   // @<<
    TK_LSL_EQ_S,                // @<<=
    
    TK_GR,  					// >
    TK_GR_EQ,				    // >=
    TK_LT,  					// <
    TK_LT_EQ,					// <=
    
    TK_CARET,                   // ^
    TK_BIT_NOT,                 // ~
    TK_BIT_AND,                 // &
    TK_BIT_OR,                  // |
    TK_LSL,                     // <<
    TK_LSR,                     // >>
    TK_BIT_XOR_EQ,              // ^=
    TK_BIT_AND_EQ,              // &=
    TK_BIT_OR_EQ,               // |=
    TK_LSL_EQ,                  // <<=
    TK_LSR_EQ,                  // >>=
    
    TK_ARROW,                   // ->
    TK_LEFT_ARROW,              // <-
    
    TK_QMRK,                    // ?
    TK_QDOT,                    // ?.
    TK_NULL_COALESCE,           // ??
    TK_NULL_COALESCE_ASSIGN,    // ??=
    TK_NULLISH_COALESCE_ASSIGN, // ||=

    TK_ASSERT, // assert
    TK_ASSERT_DB, // assert_db
    
    TK_IDENT,
    TK_STRING_LIT,
    TK_CHAR_LIT,
    TK_NUMBER,

    TK_INTERNAL_ERROR,
    TK_EOF
} NiftyTokenType;

typedef struct {
    NiftyTokenType type;
    const char *lexeme;
    int len;
    int line;
    int pos;
} Token;

typedef struct {
    char *source;
    const char *start;
    const char *current;
    char prev;
    int line;
    int linePos;
} Lexer;

Lexer *initLexer(const char *filename);
void freeLexer(Lexer *lexer);

Token nextToken(Lexer *lexer);

void printToken(Token token);

#endif //__NIFTY_LEXER_H__
