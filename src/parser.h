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

struct Node {
    Location location;
    NodeKind kind;

    union {
        struct PrototypeNode { string name; } prototypeNode;

        struct BlockNode { Nodes statements; } blockNode;
        struct ReturnNode { Node *statement; } returnNode;

        struct FunctionNode { struct PrototypeNode *prototype; struct BlockNode *body; } functionNode;

        struct AddNode { Node *left; Node *right; } addNode;
        struct SubNode { Node *left; Node *right; } subNode;
        struct MulNode { Node *left; Node *right; } mulNode;
        struct DivNode { Node *left; Node *right; } divNode;
    } data;
};

// One results per file.
typedef struct {
    conststr file;
    Nodes nodes;
    int errorCount;

    int runTime; // In ms.
} ParseResults;

typedef struct {
    Lexer *lexer;
    Token current;
    Token next;
    bool hadError;
    bool panicMode;
    string currentImpl;
    string package;

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

ParseResults *parseFile(conststr file, CompilerConfig *config);

#endif //NIFTY_PARSER_H
