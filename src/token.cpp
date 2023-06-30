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

#include "token.hpp"

namespace {
    String fmtTTS(const String &type, const String &lexeme) {
        return "<" + type + ", '" + lexeme + "'>";
    }
}

std::map<String, Tokens> Token::keywords = {
        {"let", TK_LET},
        {"val", TK_VAL},
        {"const", TK_CONST},
        {"new", TK_NEW},
        {"delete", TK_DELETE},
        {"this", TK_THIS},

        {"fn", TK_FN},
        {"md", TK_MD},

        {"return", TK_RETURN},
        {"try", TK_TRY},
        {"if", TK_IF},
        {"if!", TK_IF_NOT},
        {"else", TK_ELSE},
        {"elif", TK_ELIF},
        {"do", TK_DO},
        {"in", TK_IN},
        {"for", TK_FOR},
        {"while", TK_WHILE},
        {"until", TK_UNTIL},
        {"when", TK_WHEN},
        {"break", TK_BREAK},
        {"continue", TK_CONTINUE},
        {"goto", TK_GOTO},
        {"defer", TK_DEFER},
        {"defer_err", TK_DEFER_ERR},
        {"restrict", TK_RESTRICT},

        {"type", TK_TYPE},
        {"as", TK_AS},
        {"cast", TK_CAST},
        {"recast", TK_RECAST},
        {"auto_cast", TK_AUTOCAST},

        {"struct", TK_STRUCT},
        {"impl", TK_IMPL},
        {"contimpl", TK_CONSTIMPL},
        {"endimpl", TK_END_IMPL},
        {"does", TK_DOES},
        {"behavior", TK_BEHAVIOR},
        {"enum", TK_ENUM},

        {"and", TK_AND},
        {"or", TK_OR},
        {"true", TK_TRUE},
        {"false", TK_FALSE},

        {"size_of", TK_SIZE_OF},
        {"align_of", TK_ALIGN_OF},
        {"type_of", TK_TYPE_OF},
        {"typeid_of", TK_TYPEID_OF},
        {"typeinfo_of", TK_TYPEINFO_OF},
        {"type_from", TK_TYPE_FROM},
        {"name_of", TK_NAME_OF},

        {"int", TK_INT},
        {"uint", TK_UINT},
        {"float", TK_FLOAT},
        {"double", TK_DOUBLE},
        {"string", TK_STRING_TYPE},
        {"cstring", TK_CSTRING_TYPE},
        {"char", TK_CHAR_TYPE},
        {"bool", TK_BOOL},
        {"b8", TK_B8},
        {"b16", TK_B16},
        {"b32", TK_B32},
        {"b64", TK_B64},
        {"u8", TK_U8},
        {"u16", TK_U16},
        {"u32", TK_U32},
        {"u64", TK_U64},
        {"u128", TK_U128},
        {"s8", TK_S8},
        {"s16", TK_S16},
        {"s32", TK_S32},
        {"s64", TK_S64},
        {"s128", TK_S128},
        {"f16", TK_F16},
        {"f32", TK_F32},
        {"f64", TK_F64},
        {"f128", TK_F128},
        {"void", TK_VOID},
        {"rawptr", TK_RAWPTR},
        {"uintptr", TK_UINTPTR},
        {"typeid", TK_TYPEID},

        {"null", TK_NULL},
        {"undefined", TK_UNDEFINED},
        {"unused", TK_UNUSED},

        {"package", TK_PACKAGE},
        {"use", TK_USE},
        {"using", TK_USING},
        {"extern", TK_EXTERN},

        {"emit", TK_EMIT},
};

String tokenToString(const Token &t) {
    if (t.lexeme.empty()) {
        return "null";
    }

    let it = Token::keywords.find(t.lexeme);
    if (it != Token::keywords.end()) {
        return fmtTTS("TK_" + t.lexeme.toUpper(), t.lexeme);
    }

    switch (t.type) {
        case TK_UNKNOWN: return fmtTTS("TK_UNKNOWN", t.lexeme);
        case TK_COLON: return fmtTTS("TK_COLON", t.lexeme);
        case TK_COMMA: return fmtTTS("TK_COMMA", t.lexeme);
        case TK_SEMICOLON: return fmtTTS("TK_SEMICOLON", t.lexeme);
        case TK_LPAREN: return fmtTTS("TK_LPAREN", t.lexeme);
        case TK_RPAREN: return fmtTTS("TK_RPAREN", t.lexeme);
        case TK_LBRACE: return fmtTTS("TK_LBRACE", t.lexeme);
        case TK_RBRACE: return fmtTTS("TK_RBRACE", t.lexeme);
        case TK_LBRACKET: return fmtTTS("TK_LBRACKET", t.lexeme);
        case TK_RBRACKET: return fmtTTS("TK_RBRACKET", t.lexeme);
        case TK_DOT: return fmtTTS("TK_DOT", t.lexeme);
        case TK_C_RANGE: return fmtTTS("TK_C_RANGE", t.lexeme);
        case TK_O_RANGE: return fmtTTS("TK_O_RANGE", t.lexeme);
        case TK_VARY: return fmtTTS("TK_VARY", t.lexeme);
        case TK_LABEL: return fmtTTS("TK_LABEL", t.lexeme);
        case TK_OWNED: return fmtTTS("TK_OWNED", t.lexeme);
        case TK_MACRO: return fmtTTS("TK_MACRO", t.lexeme);
        case TK_SCOPE: return fmtTTS("TK_SCOPE", t.lexeme);
        case TK_LET_DECL: return fmtTTS("TK_LET_DECL", t.lexeme);
        case TK_VAL_DECL: return fmtTTS("TK_VAL_DECL", t.lexeme);
        case TK_ASSIGN: return fmtTTS("TK_ASSIGN", t.lexeme);
        case TK_EQU: return fmtTTS("TK_EQU", t.lexeme);
        case TK_ADD: return fmtTTS("TK_ADD", t.lexeme);
        case TK_INC: return fmtTTS("TK_INC", t.lexeme);
        case TK_ADDEQU: return fmtTTS("TK_ADDEQU", t.lexeme);
        case TK_SUB: return fmtTTS("TK_SUB", t.lexeme);
        case TK_DEC: return fmtTTS("TK_DEC", t.lexeme);
        case TK_SUBEQU: return fmtTTS("TK_SUBEQU", t.lexeme);
        case TK_MUL: return fmtTTS("TK_MUL", t.lexeme);
        case TK_MULEQU: return fmtTTS("TK_MULEQU", t.lexeme);
        case TK_DIV: return fmtTTS("TK_DIV", t.lexeme);
        case TK_DIVEQU: return fmtTTS("TK_DIVEQU", t.lexeme);
        case TK_MOD: return fmtTTS("TK_MOD", t.lexeme);
        case TK_MODEQU: return fmtTTS("TK_MODEQU", t.lexeme);
        case TK_GREATER: return fmtTTS("TK_GREATER", t.lexeme);
        case TK_GREATEREQU: return fmtTTS("TK_GREATEREQU", t.lexeme);
        case TK_LESS: return fmtTTS("TK_LESS", t.lexeme);
        case TK_LESSEQU: return fmtTTS("TK_LESSEQU", t.lexeme);
        case TK_IDENT: return fmtTTS("TK_IDENT", t.lexeme);
        case TK_STRING_LIT: return fmtTTS("TK_STRING_LIT", t.lexeme);
        case TK_CHAR_LIT:return fmtTTS("TK_CHAR_LIT", t.lexeme);
        case TK_NUMBER: return fmtTTS("TK_NUMBER", t.lexeme);
        case TK_QMRK:return fmtTTS("TK_QMRK", t.lexeme);
        case TK_QDOT: return fmtTTS("TK_QDOT", t.lexeme);
        case TK_NULL_COALESCE: return fmtTTS("TK_NULL_COALESCE", t.lexeme);
        case TK_NULL_COALESCE_ASSIGN: return fmtTTS("TK_NULL_COALESCE_ASSIGN", t.lexeme);
        case TK_NULLISH_COALESCE_ASSIGN: return fmtTTS("TK_NULLISH_COALESCE_ASSIGN", t.lexeme);
        case TK_EOF: return fmtTTS("TK_EOF", t.lexeme);
        default: return fmtTTS("???", t.lexeme);
    }
}

bool isTokenType(int type) {
    switch (type) {
        case TK_INT:
        case TK_FLOAT:
        case TK_DOUBLE:
        case TK_STRING_TYPE:
        case TK_CHAR_TYPE:
        case TK_BOOL:
        case TK_U8:
        case TK_U16:
        case TK_U32:
        case TK_U64:
        case TK_U128:
        case TK_S8:
        case TK_S16:
        case TK_S32:
        case TK_S64:
        case TK_S128:
        case TK_RAWPTR:
        case TK_UINTPTR:
        case TK_TYPEID:
            return true;
        default:
            return false;
    }
}

bool isTokenOp(int type) {
    switch (type) {
        case TK_ADD:
        case TK_ADDEQU:
        case TK_SUB:
        case TK_SUBEQU:
        case TK_MUL:
        case TK_MULEQU:
        case TK_DIV:
        case TK_DIVEQU:
        case TK_MOD:
        case TK_MODEQU:
        case TK_INC:
        case TK_DEC:
        case TK_ASSIGN:
        case TK_EQU:
        case TK_NOT:
        case TK_NOTEQU:
        case TK_LESS:
        case TK_LESSEQU:
        case TK_GREATER:
        case TK_GREATEREQU:
        case TK_AND:
        case TK_OR:
        case TK_BITAND:
        case TK_BITOR:
            return true;
        default:
            return false;
    }
}

bool isTokenBinOp(int type) {
    switch (type) {
        case TK_ADD:
        case TK_ADDEQU:
        case TK_SUB:
        case TK_SUBEQU:
        case TK_MUL:
        case TK_MULEQU:
        case TK_DIV:
        case TK_DIVEQU:
        case TK_MOD:
        case TK_MODEQU:
        case TK_ASSIGN:
        case TK_EQU:
        case TK_NOTEQU:
        case TK_LESS:
        case TK_LESSEQU:
        case TK_GREATER:
        case TK_GREATEREQU:
        case TK_AND:
        case TK_OR:
        case TK_BITAND:
        case TK_BITOR:
            return true;
        default:
            return false;
    }
}

bool isTokenOpPrePostFix(int type) {
    switch (type) {
        case TK_INC:
        case TK_DEC:
            return true;
        default:
            return false;
    }
}

bool isTokenUnaryOp(int type) {
    switch (type) {
        case TK_INC:
        case TK_DEC:
        case TK_NOT:
            return true;
        default:
            return false;
    }
}

int getTokenPrecedence(int type) {
    switch (type) {
        case TK_INC:
        case TK_DEC:
            return 100;
        case TK_NOT:
        case TK_BITNOT:
            return 90;
        case TK_MUL:
        case TK_DIV:
        case TK_MOD:
            return 80;
        case TK_ADD:
        case TK_SUB:
            return 70;
        case TK_LESS:
        case TK_LESSEQU:
        case TK_GREATER:
        case TK_GREATEREQU:
            return 60;
        case TK_EQU:
        case TK_NOTEQU:
            return 50;
        case TK_BITAND:
            return 40;
        case TK_BITOR:
            return 30;
        case TK_AND:
            return 20;
        case TK_OR:
            return 10;
        case TK_ASSIGN:
        case TK_ADDEQU:
        case TK_SUBEQU:
        case TK_MULEQU:
        case TK_DIVEQU:
        case TK_MODEQU:
            return 9;
        case TK_COMMA:
            return 1;
        default:
            return -1;
    }
}

int getNumBitsForToken(int type) {
    switch (type) {
        case TK_S8:
        case TK_U8:
            return 8;
        case TK_S16:
        case TK_U16:
            return 16;
        case TK_S32:
        case TK_U32:
        case TK_INT:
        case TK_FLOAT:
        case TK_CHAR_TYPE:
            return 32;
        case TK_S64:
        case TK_U64:
        case TK_DOUBLE:
            return 64;
        case TK_S128:
        case TK_U128:
            return 128;
        default:
            return -1;
    }
}

bool isSignedForToken(int type) {
    switch (type) {
        case TK_U8:
        case TK_U16:
        case TK_U32:
        case TK_U64:
        case TK_U128:
            return false;
        default:
            return true;
    }
}

