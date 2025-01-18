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
    
    TK_FN,
    TK_MD,

    TK_TEST,
    TK_SKIP,
    
    TK_IF,
    TK_ELIF,
    TK_ELSE,
    TK_IN,
    TK_AS,
    TK_CAST,
    TK_RECAST,
    TK_AUTO_CAST,
    TK_STRUCT,
    TK_IMPL,
    TK_CONST_IMPL,
    TK_END_IMPL,
    TK_DOES,
    TK_BEHAVIOR,
    TK_ENUM,
    
    TK_DEFER,
    TK_DEFER_ERR,
    TK_RESTRICT,
    
    TK_TYPEDEF,
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
    TK_VAL, // immutable, run time
    TK_CONST, // immutable, compile time
    TK_NEW,
    TK_DELETE,
    
    TK_USE,
    TK_USING,
    TK_NAMESPACE,
    TK_PACKAGE,
    TK_API,
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
    TK_UINTPTR,
    TK_TYPEID,
    TK_ANY_TYPE,
    
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

    TK_ASSERT,
    TK_ASSERT_DB,
    
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
