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

void errorStart(Parser *parser) {
    ParseResults *ast = parser->results;
    ast->errorCount++;
    parser->panicMode = true;

    printf("%s:L%d,C%d: ", ast->file, parser->current.line, parser->current.pos);
    setTextColor(parser->compilerConfig, ERROR_COLOR);
    printf("Parse error: ");
    setTextColor(parser->compilerConfig, RESET_COLOR);
}

void warnStart(Parser *parser) {
    ParseResults *ast = parser->results;
    ast->errorCount++;

    printf("%s:L %d,C%d: ", ast->file, parser->current.line, parser->current.pos);
    setTextColor(parser->compilerConfig, WARN_COLOR);
    printf("Warning: ");
    setTextColor(parser->compilerConfig, RESET_COLOR);
}

static void printLineWithError(Parser *parser, Token *token) {
    string line = str_get_line(parser->lexer->source, token->line, nullptr);

    int width = 3;
    for (int n = token->line; n > 0; n /= 10, ++width);

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

static void expectedAfter(Parser *parser, conststr expected, conststr after) {
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

static void errorAt(Parser *parser, Token *token, conststr msg) {
    errorStart(parser);
    println(msg);
    printLineWithError(parser, token);
}

static void errorAtCurrent(Parser *parser, conststr msg) {
    errorAt(parser, &parser->current, msg);
}

static void advance(Parser *parser) {
    parser->current = parser->next;
    parser->next = nextToken(parser->lexer);
}

static void eat(Parser *parser, NiftyTokenType tokenType, conststr msg) {
    if (parser->current.type == tokenType) {
        advance(parser);
        return;
    }

    errorAtCurrent(parser, msg);
}

static Parser *initParser(conststr file, CompilerConfig *config) {
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
    parser->package = nullptr;
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

static bool check(Parser *parser, NiftyTokenType tokenType) {
    return parser->current.type == tokenType;
}

static bool match(Parser *parser, NiftyTokenType tokenType) {
    if (!check(parser, tokenType)) {
        return false;
    }
    advance(parser);

    return true;
}

static void packageDeclaration(Parser *parser) {
    if (parser->package != nullptr) {
        errorAtCurrent(parser, "Package already set on line bla.");
    }

    if (!match(parser, TK_IDENT)) {
        expectedAfter(parser, "identifier", "package");
        return;
    }

    parser->package = str_new_len(parser->current.lexeme, parser->current.len);
//    eat(parser, TK_IDENT, "Expected identifier after package.");
}

static void fnDeclaration(Parser *parser) {
    //
}

static void declaration(Parser *parser) {
    if (match(parser, TK_PACKAGE)) {
        packageDeclaration(parser);
    } else if (match(parser, TK_FN) || match(parser, TK_MD)) {
        fnDeclaration(parser);
    } else {
        advance(parser); // TODO: Remove.
    }
}

ParseResults *parseFile(conststr file, CompilerConfig *config) {
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
