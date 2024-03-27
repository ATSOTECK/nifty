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

static void advance(Parser *parser) {
    parser->current = parser->next;
    parser->next = nextToken(parser->lexer);
}

static Parser *initParser(conststr file, CompilerConfig *config) {
    Parser *parser = (Parser*)malloc(sizeof(Parser));
    Ast *ast = (Ast*)malloc(sizeof(Ast));

    if (parser == nullptr || ast == nullptr) {
        println("Out of memory.");
        return nullptr;
    }

    ast->errorCount = 0;
    ast->nodes.count = 0;
    ast->nodes.capacity = 32;
    ast->nodes.list = (Node**)malloc(sizeof(Node) * 32);
    ast->file = str_new(file, nullptr);

    parser->ast = ast;
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

void errorStart(Parser *parser) {
    Ast *ast = parser->ast;
    ast->errorCount++;

    printf("%s:L %d,C%d: ", ast->file, parser->current.line, parser->current.pos);
    if (!parser->compilerConfig->disableColors) {
        printf(ERROR_COLOR);
    }
    printf("Parser error: ");
    if (!parser->compilerConfig->disableColors) {
        printf(RESET_COLOR);
    }
}

void warnStart(Parser *parser) {
    Ast *ast = parser->ast;
    ast->errorCount++;

    printf("%s:L %d,C%d: ", ast->file, parser->current.line, parser->current.pos);
    if (!parser->compilerConfig->disableColors) {
        printf(WARN_COLOR);
    }
    printf("Warning: ");
    if (!parser->compilerConfig->disableColors) {
        printf(RESET_COLOR);
    }
}

Ast *parseFile(conststr file, CompilerConfig *config) {
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
        free(parser->ast->nodes.list);
        free(parser->ast);
        freeParser(parser);
        return nullptr;
    }
    
    while (parser->current.type != TK_EOF) {
        printToken(parser->current);
        advance(parser);
    }

    Ast *ast = parser->ast;
    freeParser(parser);

    return ast;
}
