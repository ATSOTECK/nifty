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

#ifndef __NIFTY_PARSER_HPP__
#define __NIFTY_PARSER_HPP__

#include "common.hpp"
#include "lexer.hpp"
#include "node.hpp"
#include "symbolTable.hpp"

#include <vector>

class Parser {
public:
    explicit Parser(Lexer *lex);

    keep bool finishedWithErrors() const {
        return _finishedWithErrors;
    }

    keep bool foundEntrypoint() const {
        return _foundEntrypoint;
    }

    void firstEat();
    void eat();
    void eat(int type, const String &expected, const String &after);
    void eat(uint8 amount);

    bool check(int type);
    bool lookahead(int type);
    bool match(int type);

    Token lookaheadToken(uint8 t);
    uint8 lookaheadTokenType(uint8 t);

    void errInit();
    void warnInit();
    void printLineWithError();
    Node *parseError(const String &expected, const String &after);
    Node *parseErr(const char *error, ...);
    Node *error(const String &error, const String &token);
    Node *error(const String &error);
    void warning(const String &warning);

    std::vector<Node *> parse();

private:
    Node *parsePrimary();
    void parsePackage();
    Node *parseExpression();
    Node *parseBinOpRhs(int precedence, Node *lhs);
    Node *parseNumber();
    Node *parseType();
    PrototypeNode *parsePrototype(const String &name);
    Node *parseFunction();

    Lexer *_lex;
    Token _lookahead;
    Token _current;
    int _lookaheadLine;
    int _line;
    int _lookaheadPos;
    int _pos;

    std::vector<Node *> _nodes;
    String _currentPackage;

    bool _finishedWithErrors;
    bool _foundEntrypoint;
};


#endif //__NIFTY_PARSER_HPP__
