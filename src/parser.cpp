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
}

void Parser::eat(uint8 amount) {
    for (int i = 0; i < amount; ++i) {
        eat();
    }
}

void Parser::errInit() {
    _finishedWithErrors = true;

    std::cerr << _lex->filename() << ":L" << _current.line << ",C" << _current.pos << ": ";
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
    FILE *file = 0;
    file = fopen(_lex->path().c_str(), "r");

    if (file != nullptr) {
        int lineNumber = 0;
        char line[4096]; //TODO: Dynamic line size.
        while (fgets(line, sizeof line, file) != nullptr) {
            if (++lineNumber == _current.line) {
                break;
            }
        }

        fclose(file);

        std::cerr << line;
        setCmdColor(GreenColor);
        for (int i = 0; i < _current.pos - 1; ++i) {
            std::cerr << " ";
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
    std::cerr << " expected ";
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

    return nullptr;
}
