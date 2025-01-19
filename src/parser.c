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

#include "parser.h"

#include <stdlib.h>
#include <__stdarg_va_arg.h>

#include "util/str.h"

void errorStart(Parser *parser) {
    ParseResults *ast = parser->results;
    ast->errorCount++;
    parser->panicMode = true;

    printf("%s:L%d,C%d: ", ast->file, parser->current.line, parser->current.pos);
    setTextColor(parser->compilerConfig, ERROR_COLOR);
    printf("Parse error: ");
    setTextColor(parser->compilerConfig, RESET_COLOR);
}

void warnStart(const Parser *parser) {
    ParseResults *ast = parser->results;
    ast->errorCount++;

    printf("%s:L %d,C%d: ", ast->file, parser->current.line, parser->current.pos);
    setTextColor(parser->compilerConfig, WARN_COLOR);
    printf("Warning: ");
    setTextColor(parser->compilerConfig, RESET_COLOR);
}

static void printLineWithError(const Parser *parser, const Token *token) {
    char *line = str_get_line(parser->lexer->source, token->line, nullptr);

    int width = 3;
    for (int n = token->line; n > 0; n /= 10, ++width) {}

    setTextColor(parser->compilerConfig, LINE_COLOR);
    printf("%d | ", token->line);
    setTextColor(parser->compilerConfig, RESET_COLOR);
    println("%s", line);
    setTextColor(parser->compilerConfig, LINE_COLOR);

    int i = 0;
    for (; i < width - 2; ++i) {
        printf(" ");
    }
    printf("| ");
    for (; i - width + 3 < token->pos; ++i) {
        printf("~");
    }
    println("^");
    setTextColor(parser->compilerConfig, RESET_COLOR);
}

static void expectedAfter(Parser *parser, const char *expected, const char *after) {
    errorStart(parser);
    printf("Expected ");
    setTextColor(parser->compilerConfig, HIGHLIGHT_COLOR);
    printf("%s", expected);
    setTextColor(parser->compilerConfig, RESET_COLOR);
    printf(" after ");
    setTextColor(parser->compilerConfig, HIGHLIGHT_COLOR);
    printf("%s", after);
    setTextColor(parser->compilerConfig, RESET_COLOR);
    printf(", got ");
    setTextColor(parser->compilerConfig, HIGHLIGHT_COLOR);
    printf("%.*s", parser->current.len, parser->current.lexeme);
    setTextColor(parser->compilerConfig, RESET_COLOR);
    println(" instead.");
    printLineWithError(parser, &parser->current);
}

static void errorAt(Parser *parser, const Token *token, const char *msg) {
    errorStart(parser);
    println(msg);
    printLineWithError(parser, token);
}

static void errorAtCurrent(Parser *parser, const char *msg) {
    errorAt(parser, &parser->current, msg);
}

static void errorAtCurrentf(Parser *parser, const char *msg, ...) {
    char *buf = (char *)malloc(sizeof(char) * 1024);
    va_list args;
    va_start(args, msg);
    vsprintf(buf, msg, args);
    va_end(args);

    errorAt(parser, &parser->current, buf);
    free(buf);
}

static Node *newNode(const Parser *parser, const NodeKind kind) {
    Node *node = (Node *)malloc(sizeof(Node));
    node->location.line = parser->current.line;
    node->location.pos = parser->current.pos;
    node->kind = kind;

    return node;
}

static void addNode(const Parser *parser, Node *node) {
    if (parser->results->nodes.count + 1 >= parser->results->nodes.capacity) {
        parser->results->nodes.capacity *= 2;
        parser->results->nodes.list = (Node**)malloc(sizeof(Node) * parser->results->nodes.capacity);
    }

    parser->results->nodes.list[parser->results->nodes.count++] = node;
}

static void freeNodes(const Nodes nodes) {
    for (int i = 0; i < nodes.count; ++i) {
        free(nodes.list[i]);
    }
    free(nodes.list);
}

void freeParseResults(ParseResults *results) {
    if (results == nullptr) {
        return;
    }

    freeNodes(results->nodes);
    free(results);
}

static void advance(Parser *parser) {
    parser->current = parser->next;
    parser->next = nextToken(parser->lexer);
}

static void eat(Parser *parser, const NiftyTokenType tokenType, const char *msg) {
    if (parser->current.type == tokenType) {
        advance(parser);
        return;
    }

    errorAtCurrent(parser, msg);
}

static Parser *initParser(const char *file, CompilerConfig *config) {
    Parser *parser = (Parser*)malloc(sizeof(Parser));
    ParseResults *results = (ParseResults*)malloc(sizeof(ParseResults));

    if (parser == nullptr || results == nullptr) {
        println("Out of memory.");
        return nullptr;
    }

    results->errorCount = 0;
    results->nodes.count = 0;
    results->nodes.capacity = 32;
    results->nodes.list = (Node**)malloc(sizeof(Node) * 32);
    results->file = str_new(file, nullptr);

    parser->hadError = false;
    parser->panicMode = false;
    parser->currentImpl = nullptr;
    parser->namespace.name = nullptr;
    parser->namepsaceLine = -1;
    parser->results = results;
    parser->compilerConfig = config;
    parser->lexer = initLexer(file);
    parser->next = nextToken(parser->lexer);
    advance(parser);

    return parser;
}

static void freeParser(Parser *parser) {
    freeLexer(parser->lexer);
    free(parser);
}

static bool check(const Parser *parser, const NiftyTokenType tokenType) {
    return parser->current.type == tokenType;
}

static bool match(Parser *parser, const NiftyTokenType tokenType) {
    if (!check(parser, tokenType)) {
        return false;
    }
    advance(parser);

    return true;
}

static void namespaceDeclaration(Parser *parser) {
    if (parser->namespace.name != nullptr) {
        errorAtCurrentf(parser, "Namespace already set on line %d.", parser->namepsaceLine);
        return;
    }

    parser->namepsaceLine = parser->current.line;

    if (!check(parser, TK_IDENT)) {
        expectedAfter(parser, "identifier", "namespace");
        return;
    }

    parser->namespace.name = str_new_len(parser->current.lexeme, parser->current.len);
    advance(parser);
//    eat(parser, TK_IDENT, "Expected identifier after package.");
}

static BlockNode *parseBlock(const Parser *parser) {
    return nullptr;
}

static ArgNode **parseArguments(Parser *parser) {
    eat(parser, TK_LPAREN, "Expected (");

    while (parser->current.type != TK_RPAREN) {
        advance(parser);
    }

    eat(parser, TK_RPAREN, "Expected )");

    return nullptr;
}

static TypeNode **parseReturnTypes(Parser *parser) {
    return nullptr;
}

static PrototypeNode *parsePrototype(Parser *parser) {
    if (!check(parser, TK_IDENT)) {
        expectedAfter(parser, "identifier", "fn"); // TODO: Handle md.
        return nullptr;
    }

    PrototypeNode *prototype = (PrototypeNode*)malloc(sizeof(PrototypeNode));
    prototype->name = str_new_len(parser->current.lexeme, parser->current.len);
    advance(parser);

    prototype->args = parseArguments(parser);
    prototype->returnTypes = parseReturnTypes(parser);

    return prototype;
}

static void fnDeclaration(Parser *parser) {
    Node *function = newNode(parser, FunctionNodeType);
    function->data.functionNode.prototype = parsePrototype(parser);
    function->data.functionNode.body = parseBlock(parser);
    addNode(parser, function);
}

static void declaration(Parser *parser) {
    if (match(parser, TK_NAMESPACE)) {
        namespaceDeclaration(parser);
    } else if (match(parser, TK_FN) || match(parser, TK_MD)) {
        fnDeclaration(parser);
    } else {
        advance(parser); // TODO: Remove.
    }
}

ParseResults *parseFile(const char *file, CompilerConfig *config) {
    if (config == nullptr) {
        println("Invalid compiler config sent to parser.");
        return nullptr;
    }

    Parser *parser = initParser(file, config);
    if (parser == nullptr) {
        println("Could not initialize parser.");
        return nullptr;
    }
    
    if (parser->lexer == nullptr) {
        free(parser->results->nodes.list);
        free(parser->results);
        freeParser(parser);
        return nullptr;
    }

    while (!match(parser, TK_EOF)) {
        declaration(parser);
    }
    
//    while (parser->current.type != TK_EOF) {
//        printToken(parser->current);
//        advance(parser);
//    }

    ParseResults *results = parser->results;
    freeParser(parser);

    return results;
}
