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

    use_fn finishedWithErrors() const -> bool {
        return _finishedWithErrors;
    }

    use_fn foundEntrypoint() const -> bool {
        return _foundEntrypoint;
    }

    fn firstEat() -> void;
    fn eat() -> void;
    fn expectAfter(int type, const String &expected, const String &after) -> void;
    fn expect(int type, const String &expected) -> void;
    fn expectSemicolon() -> void;
    fn maybeSemicolon() -> void;
    fn eat(uint8 amount) -> void;

    use_fn check(int type) const -> bool;
    use_fn lookahead(int type) const -> bool;
    fn match(int type) -> bool;

    fn lookaheadToken(uint8 t) -> Token;
    fn lookaheadTokenType(uint8 t) -> uint8;

    fn errInit() -> void;
    fn warnInit() -> void;
    fn printLineWithError(const Token &token) -> void;
    fn parseError(const String &expected, const String &after) -> Node*;
    fn parseError(const String &expected) -> Node*;
    fn redefinitionErrorArg(const String &arg) -> void;
    fn parseErr(const char *error, ...) -> Node*;
    fn error(const String &error, const String &token) -> Node*;
    fn error(const String &error) -> Node*;
    fn warning(const String &warning);

    fn parse() -> Nodes;

private:
    fn parsePrimary() -> Node*;
    fn parsePackage() -> void;
    fn parseExpression() -> Node*;
    fn parseBinOpRhs(int precedence, Node *lhs) -> Node*;
    fn parseNumber() -> Node*;
    fn parseBool(bool value) -> Node*;
    fn parseType() -> Node*;
    fn parsePrototype(const String &name) -> PrototypeNode*;
    fn parseFunction() -> Node*;
    fn parseReturn() -> Node*;

    Lexer *_lex;
    Token _lookahead;
    Token _current;
    Token _previous;

    Nodes _nodes;
    String _currentPackage;

    bool _finishedWithErrors;
    bool _foundEntrypoint;
    
    bool _inFunction;
    String _currentFnName;
};


#endif //__NIFTY_PARSER_HPP__
