/*
 * Nifty - Nifty Programming Language
 * Copyright (c) 2022 Skyler Burwell
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

#include "parser.hpp"

#include <iostream>

Parser::Parser(Lexer *lex):
    _lex(lex),
    _lookaheadLine(0),
    _line(0),
    _lookaheadPos(0),
    _pos(0),
    _finishedWithErrors(false),
    _foundEntrypoint(false)
{
    //
}

void Parser::firstEat() {
    _lookahead = _lex->nextToken();
    _lookaheadLine = _lex->line();
    _lookaheadPos = _lex->linePos();

    eat();
}

void Parser::eat() {
    _current = _lookahead;
    _line = _lookaheadLine;
    _pos = _lookaheadPos;

    _lookahead = _lex->nextToken();
    _lookaheadLine = _lex->line();
    _lookaheadPos = _lex->linePos();

    String tok = tokenToString(_current);
    db(tok);
}

void Parser::eat(int type, const String &expected, const String &after) {
    eat();

    if (_current.type == type) {
        return;
    }

    parseError(expected, after);
}

void Parser::eat(uint8 amount) {
    for (int i = 0; i < amount; ++i) {
        eat();
    }
}

bool Parser::check(int type) {
    return _current.type == type;
}

bool Parser::lookahead(int type) {
    return _lookahead.type == type;
}

bool Parser::match(int type) {
    if (!check(type)) {
        return false;
    }

    eat();
    return true;
}

void Parser::errInit() {
    _finishedWithErrors = true;

    std::cerr << _lex->filename() << " L" << _current.line << ",C" << _current.pos << ": ";
    setCmdColor(ErrorColor);
    std::cerr << "Parse error: ";
    setCmdColor(TextColor);
}

void Parser::warnInit() {
    std::cerr << _lex->filename() << ":L" << _current.line << ",C" << _current.pos << ": ";
    setCmdColor(WarnColor);
    std::cerr << "Warning: ";
    setCmdColor(TextColor);
}

void Parser::printLineWithError() {
    FILE *file = fopen(_lex->path().c_str(), "r");

    if (file != nullptr) {
        int lineNumber = 0;
        char line[4096]; //TODO: Dynamic line size.
        while (fgets(line, sizeof line, file) != nullptr) {
            if (++lineNumber == _current.line) {
                break;
            }
        }

        fclose(file);

        int width = 3;
        for (int n = _current.line; n > 0; n /= 10) {
            ++width;
        }

        setCmdColor(GreenColor);
        std::cerr << _current.line << " | ";
        setCmdColor(TextColor);
        std::cerr << line;
        setCmdColor(GreenColor);
        int i = 0;
        for (; i < width - 2; ++i) {
            std::cerr << " ";
        }
        std::cerr << "| ";
        for (; i - width + 2 < _current.pos; ++i) {
            std::cerr << "~";
        }
        std::cerr << "^" << std::endl;
        setCmdColor(TextColor);

    } else {
        setCmdColor(ErrorColor);
        std::cerr << "Error";
        setCmdColor(TextColor);
        std::cerr << " unable to open \"" << _lex->path() << "\".";
    }
}

Node *Parser::parseError(const String &expected, const String &after) {
    errInit();
    std::cerr << " Expected ";
    setCmdColor(PurpleColor);
    std::cerr << "'" << expected << "'";
    setCmdColor(TextColor);
    std::cerr << " after ";
    setCmdColor(PurpleColor);
    std::cerr << "\"" << after << "\"";
    setCmdColor(TextColor);
    std::cerr << " got ";
    setCmdColor(PurpleColor);
    std::cerr << tokenToString(_current);
    setCmdColor(TextColor);
    std::cerr << " instead." << std::endl;
    printLineWithError();

    return nullptr;
}

Node *Parser::parseErr(const char *error, ...) {
    errInit();

    char buff[2048];
    va_list args;
    va_start(args, error);
    vsprintf(buff, error, args);
    va_end(args);
    std::cerr << buff << std::endl;

    printLineWithError();

    return nullptr;
}

Node *Parser::error(const String &error, const String &token) {
    errInit();
    std::cerr << error << " \"" << token << "\"." << std::endl;
    printLineWithError();

    return nullptr;
}

Node *Parser::error(const String &error) {
    errInit();
    std::cerr << "on line " << _line << ": " << error << std::endl;
    printLineWithError();

    return nullptr;
}

void Parser::warning(const String &warning) {
    warnInit();
    std::cerr << "on line " << _line << ": " << warning << std::endl;
    printLineWithError();
}

std::vector<Node *> Parser::parse() {
    firstEat();

    while (_current.type != TK_EOF) {
        Node *expr = parsePrimary();

        if (expr == nullptr) {
            return _nodes;
        }

        _nodes.push_back(expr);
    }

    return _nodes;
}

Node *Parser::parsePrimary() {
    if (_current.type == TK_EOF) {
        return nullptr;
    }

    if (_current.type == TK_SEMICOLON) {
        eat();
    }

    if (isTokenType(_current.type)) {
        return new TypeNode(_current.lexeme);
    }

    switch (_current.type) {
        case TK_NUMBER: return parseNumber();
        case TK_FN: return parseFunction();
    }

    return nullptr;
}

Node *Parser::parseExpression() {
    Node *lhs = parsePrimary();
    if (lhs == nullptr) {
        return nullptr;
    }

    return parseBinOpRhs(0, lhs);
}

Node *Parser::parseBinOpRhs(int precedence, Node *lhs) {
    for (;;) {
        int tokenPrec = getTokenPrecedence(_current.type);

        if (tokenPrec < precedence) {
            return lhs;
        }

        Token binOp = _current;
        eat();

        /*
        if (isTokenOpPrePostFix(binOp.type)) {
            if (binOp.type == TK_INC) {
                return new PostIncNode(lhs);
            } else {
                return new PostDecNode(lhs);
            }
        }
        */

        Node *rhs = parsePrimary();
        if (rhs == nullptr) {
            return nullptr;
        }

        int nextPrec = getTokenPrecedence(_current.type);
        if (tokenPrec < nextPrec) {
            rhs = parseBinOpRhs(tokenPrec + 1, rhs);
            if (rhs == nullptr) {
                return nullptr;
            }
        }

        // TODO: Fix this.
        lhs = new BinaryNode(binOp, lhs, rhs);
    }
}

Node *Parser::parseNumber() {
    String num = _current.lexeme;

    eat();

    if (num.contains('d') || num.contains('D')) {
        return new FloatNode(8, num);
    } else if (num.contains('f') || num.contains('F') || num.contains('.')) {
        return new FloatNode(4, num);
    }

    bool isSigned = true;
    if (num.contains('u') || num.contains('U')) {
        isSigned = false;
    }

    return new IntNode(4, num, isSigned);
}

PrototypeNode *Parser::parsePrototype(String name) {
    int arity = 0;
    std::vector<Node *> args;
    std::vector<Node *> returnTypes;
    if (!check(TK_RPAREN)) {
        do {
            if (++arity > 32) {
                error("Can't have more than 32 arguments.");
            }

            //TODO: name: type, etc
            //TODO: name: type = value, etc
        } while (match(TK_COMMA));
    }

    eat(); // Eat the )
    eat(TK_COLON, ":", ")");

    if (check(TK_LBRACE)) {
        returnTypes.push_back(new VoidNode());
    } else {

    }

    return nullptr;
}

Node *Parser::parseFunction() {
    eat(); // Eat the fn

    if (!check(TK_IDENT)) {
        return parseError("name", "fn");
    }

    String name = _current.lexeme;

    eat(TK_LPAREN, "(", "name");
    PrototypeNode *prototype = parsePrototype(name);

    return nullptr;
}
