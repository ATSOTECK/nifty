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

#ifndef NIFTY_PARSER_H
#define NIFTY_PARSER_H

#include "lexer.h"

#include "common.h"

typedef enum {
    TYPE_U8,
    TYPE_U16,
    TYPE_U32,
    TYPE_U64,
    TYPE_U28,
    TYPE_S8,
    TYPE_S16,
    TYPE_S32,
    TYPE_S64,
    TYPE_S28,
    TYPE_F32,
    TYPE_F64,
    TYPE_F128,
    TYPE_B8,
    TYPE_B16,
    TYPE_B32,
    TYPE_B64,

    TYPE_STRING,
    TYPE_CHAR,

    TYPE_ARRAY,
    TYPE_SLICE,
    TYPE_STRUCT,
    TYPE_ENUM,

    TYPE_POINTER,
    TYPE_UINTPTR,

    TYPE_TYPE_ID,

    TYPE_ANY,
    TYPE_NONE,
} TypeKind;

typedef struct Node Node;

typedef struct {
    int count;
    int capacity;
    Node **list;
} Nodes;

typedef enum {
    VoidNodeType,

    PrototypeNodeType,

    BlockNodeType,
    ReturnNodeType,

    FunctionNodeType,

    AddNodeType,
    SubNodeType,
    MulNodeType,
    DivNodeType,
} NodeKind;

typedef struct {
    int line;
    int pos;
} Location;

typedef struct TypeNode { char *name; TypeKind typeKind; } TypeNode;
typedef struct ArgNode { char *name; TypeNode type; } ArgNode;
typedef struct PrototypeNode { char *name; ArgNode **args; TypeNode **returnTypes; } PrototypeNode;
typedef struct BlockNode { Nodes statements; } BlockNode;
typedef struct ReturnNode { Node *statement; } ReturnNode;
typedef struct FunctionNode { PrototypeNode *prototype; BlockNode *body; } FunctionNode;

typedef struct AddNode { Node *left; Node *right; } AddNode;
typedef struct SubNode { Node *left; Node *right; } SubNode;
typedef struct MulNode { Node *left; Node *right; } MulNode;
typedef struct DivNode { Node *left; Node *right; } DivNode;

struct Node {
    Location location;
    NodeKind kind;

    union {
        TypeNode typeNode;
        ArgNode argNode;
        PrototypeNode prototypeNode;

        BlockNode blockNode;
        ReturnNode returnNode;

        FunctionNode functionNode;

        AddNode addNode;
        SubNode subNode;
        MulNode mulNode;
        DivNode divNode;
    } data;
};

// One results per file.
typedef struct {
    const char *file;
    Nodes nodes;
    int errorCount;

    int runTime; // In ms.
} ParseResults;

typedef struct {
    char *name;
    // List of files
    // Symbol table
} Namespace;

typedef struct {
    Lexer *lexer;
    Token current;
    Token next;
    bool hadError;
    bool panicMode;
    char *currentImpl;
    Namespace namespace;
    int namepsaceLine;

    ParseResults *results;
    CompilerConfig *compilerConfig;
} Parser;

typedef struct {
    Token name;
    int depth;
    bool isConst;
    bool isMutated; // If not const and not mutated warning on debug, error on release.
    bool unused; // If unused warning on debug, error on release.
} Local;

ParseResults *parseFile(const char *file, CompilerConfig *config);
void freeParseResults(ParseResults *results);

#endif //NIFTY_PARSER_H
