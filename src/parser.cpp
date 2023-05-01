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

#include "parser.hpp"

#include <iostream>

Parser::Parser(Lexer *lex):
    _lex(lex),
    _finishedWithErrors(false),
    _foundEntrypoint(false),
    _inFunction(false)
{
    //
}

fn Parser::firstEat() -> void {
    _lookahead = _lex->nextToken();

    eat();
}

fn Parser::eat() -> void {
    _previous = _current;
    _current = _lookahead;

    _lookahead = _lex->nextToken();
    String tok = tokenToString(_current);
    db(tok);
}

fn Parser::expectAfter(int type, const String &expected, const String &after) -> void {
    if (_lookahead.type != type) {
        parseError(expected, after);
        return;
    }

    eat();
}

fn Parser::expect(int type, const String &expected) -> void {
    if (_lookahead.type != type) {
        parseError(expected);
        return;
    }

    eat();
}

fn Parser::expectSemicolon() -> void {
    if (_current.type == TK_SEMICOLON) {
        eat();
        return;
    }

    errInit();
    std::cerr << " Expected ";
    setCmdColor(PurpleColor);
    std::cerr << "';'";
    setCmdColor(TextColor);
    std::cerr << " after ";
    setCmdColor(PurpleColor);
    std::cerr << "'" << _previous.lexeme << "'.";
    setCmdColor(TextColor);
    std::cerr << std::endl;
    printLineWithError(_previous);
}

fn Parser::maybeSemicolon() -> void {
    if (_current.type == TK_SEMICOLON) {
        eat();
    }
}

fn Parser::eat(uint8 amount) -> void {
    for (int i = 0; i < amount; ++i) {
        eat();
    }
}

fn Parser::check(int type) const -> bool {
    return _current.type == type;
}

fn Parser::lookahead(int type) const -> bool {
    return _lookahead.type == type;
}

fn Parser::match(int type) -> bool {
    if (!check(type)) {
        return false;
    }

    eat();
    return true;
}

fn Parser::errInit() -> void {
    _finishedWithErrors = true;

    std::cerr << _lex->filename() << " L" << _current.line << ",C" << _current.pos << ": ";
    setCmdColor(ErrorColor);
    std::cerr << "Parse error: ";
    setCmdColor(TextColor);
}

fn Parser::warnInit() -> void {
    std::cerr << _lex->filename() << ":L" << _current.line << ",C" << _current.pos << ": ";
    setCmdColor(WarnColor);
    std::cerr << "Warning: ";
    setCmdColor(TextColor);
}

fn Parser::printLineWithError(const Token &token) -> void {
    FILE *file = fopen(_lex->path().c_str(), "r");

    if (file != nullptr) {
        int lineNumber = 0;
        char line[4096]; //TODO: Dynamic line size.
        while (fgets(line, sizeof line, file) != nullptr) {
            if (++lineNumber == token.line) {
                break;
            }
        }

        fclose(file);

        int width = 3;
        for (int n = token.line; n > 0; n /= 10) {
            ++width;
        }

        setCmdColor(GreenColor);
        std::cerr << token.line << " | ";
        setCmdColor(TextColor);
        std::cerr << line;
        setCmdColor(GreenColor);
        int i = 0;
        for (; i < width - 2; ++i) {
            std::cerr << " ";
        }
        std::cerr << "| ";
        for (; i - width + 2 < token.pos; ++i) {
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

fn Parser::parseError(const String &expected, const String &after) -> Node* {
    errInit();
    std::cerr << " Expected ";
    setCmdColor(PurpleColor);
    std::cerr << "'" << expected << "'";
    setCmdColor(TextColor);
    std::cerr << " after ";
    setCmdColor(PurpleColor);
    std::cerr << "'" << after << "'";
    setCmdColor(TextColor);
    std::cerr << " got ";
    setCmdColor(PurpleColor);
    std::cerr << tokenToString(_lookahead);
    setCmdColor(TextColor);
    std::cerr << " instead." << std::endl;
    printLineWithError(_current);

    return nullptr;
}

fn Parser::parseError(const String &expected) -> Node* {
    errInit();
    std::cerr << " Expected ";
    setCmdColor(PurpleColor);
    std::cerr << "'" << expected << "'." << std::endl;
    setCmdColor(TextColor);
    printLineWithError(_current);

    return nullptr;
}

fn Parser::redefinitionErrorArg(const String &arg) -> void {
    errInit();
    std::cerr << "Redefinition of argument ";
    setCmdColor(PurpleColor);
    std::cerr << "'" << arg << "'.";
    setCmdColor(TextColor);
    printLineWithError(_current);
}

fn Parser::parseErr(const char *error, ...) -> Node* {
    errInit();

    char buff[2048];
    va_list args;
    va_start(args, error);
    vsnprintf(buff, 2048, error, args);
    va_end(args);
    std::cerr << buff << std::endl;

    printLineWithError(_current);

    return nullptr;
}

fn Parser::error(const String &error, const String &token) -> Node* {
    errInit();
    std::cerr << error << " \"" << token << "\"." << std::endl;
    printLineWithError(_current);

    return nullptr;
}

fn Parser::error(const String &error) -> Node* {
    errInit();
    std::cerr << "on line " << _current.line << ": " << error << std::endl;
    printLineWithError(_current);

    return nullptr;
}

fn Parser::warning(const String &warning) {
    warnInit();
    std::cerr << "on line " << _current.line << ": " << warning << std::endl;
    printLineWithError(_current);
}

fn Parser::parse() -> Nodes {
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

fn Parser::parsePrimary() -> Node* {
    if (_current.type == TK_EOF) {
        return nullptr;
    }

    if (_current.type == TK_SEMICOLON) {
        eat();
    }

    if (isTokenType(_current.type)) {
        return newType(_current.lexeme);
    }
    
    if (_current.type == TK_PACKAGE) {
        parsePackage();
    }

    switch (_current.type) {
        case TK_NUMBER: return parseNumber();
        case TK_FN: return parseFunction();
        case TK_TRUE: return parseBool(true);
        case TK_FALSE: return parseBool(false);
    }

    return nullptr;
}

fn Parser::parsePackage() -> void {
    expectAfter(TK_IDENT, "identifier", "package");
    std::cout << "current package is now: " << _current.lexeme << std::endl;
    _currentPackage = _current.lexeme;
    eat(); // Eat the package name.
    let *package = new SymbolTable();
    // expectSemicolon();
    maybeSemicolon();
}

fn Parser::parseExpression() -> Node* {
    Node *lhs = parsePrimary();
    if (lhs == nullptr) {
        return nullptr;
    }

    return parseBinOpRhs(0, lhs);
}

fn Parser::parseUnary() -> Node * {
    if (!isTokenType(_current.type)) {
        return parsePrimary();
    }
    
    if (_current.type == TK_INC) {
        eat();
        return newIncDec(true, true, parsePrimary());
    }
    if (_current.type == TK_DEC) {
        eat();
        return newIncDec(false, true, parsePrimary());
    }
    
    Token opcode = _current;
    eat();
    
    Node *operand = parsePrimary();
    if (operand != nullptr) {
        return newUnary(opcode, operand);
    }
    
    return nullptr;
}

fn Parser::parseBinOpRhs(int precedence, Node *lhs) -> Node* {
    for (;;) {
        int tokenPrec = getTokenPrecedence(_current.type);

        if (tokenPrec < precedence) {
            return lhs;
        }

        Token binOp = _current;
        eat();
        
        if (binOp.type == TK_INC) {
            return newIncDec(true, false, lhs);
        }
        if (binOp.type == TK_DEC) {
            return newIncDec(false, false, lhs);
        }

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
        lhs = newBinary(binOp, lhs, rhs);
    }
}

fn Parser::parseNumber() -> Node* {
    String num = _current.lexeme;

    eat();

    // TODO: Hex, Oct, Bin.
    if (num.contains('d') || num.contains('D')) {
        return newFloat(64, num);
    } else if (num.contains('f') || num.contains('F') || num.contains('.')) {
        return newFloat(32, num);
    }

    bool isSigned = true;
    if (num.contains('u') || num.contains('U')) {
        isSigned = false;
    }

    return newInt(32, num, isSigned);
}

fn Parser::parseBool(bool value) -> Node* {
    return newBool(32, value);
}

fn Parser::parseBlock() -> BlockNode* {
    eat(); // eat the '{'
    
    Nodes statements;
    while (_current.type != TK_RBRACE) {
        let statement = parsePrimary();
        if (statement == nullptr) {
            return nullptr;
        }
        
        statements.push_back(statement);
    }
    
    eat(); // eat the '}'
    return newBlock(statements);
}

fn Parser::parsePtrType() -> Node* {
    return nullptr;
}

fn Parser::parseFnType() -> Node* {
    return nullptr;
}

fn Parser::parseArrayType() -> Node* {
    return nullptr;
}

fn Parser::parseType() -> Node* {
    // name
    // ^name
    // fn(type, type, ...): type
    // []name
    // type_from(name)
    // check if type name is known
    
    if (_current.type == TK_CARET) {
        return parsePtrType();
    }
    if (_current.type == TK_FN) {
        return parseFnType();
    }
    if (_current.type == TK_LBRACKET) {
        return parseArrayType();
    }
    
    // other types here
    
    return nullptr;
}

fn Parser::parsePrototype(const String &name) -> PrototypeNode* {
    int arity = 0;
    Nodes args;
    Nodes returnTypes;
    std::vector<String> argNames;
    if (!check(TK_RPAREN)) {
        while (_current.type != TK_RPAREN) {
            expect(TK_IDENT, "name");
            String argName = _current.lexeme; // TODO: Check if this exists already.
            eat();

            if (std::find(argNames.begin(), argNames.end(), argName) != argNames.end()) {
                redefinitionErrorArg(argName);
                return nullptr;
            }


            if (++arity > 16) {
                error("A function can't have more than 16 arguments.");
            }
            
            if (_current.type == TK_COLON) {
                let arg = parseExpression(); // parseType();
                if (arg != nullptr) {
                    args.push_back(arg);
                }
            } else if (_current.type == TK_COMMA) {
                // name, name: type
            } else if (_current.type == TK_LET_DECL) {
                // name := value
            } else {
                parseError("':', or, ':=', or ','", "identifier");
            }

            //TODO: name: type, etc
            //TODO: name: type = value, etc
            //TODO: name := value, etc
            //TODO: name, name: type, etc
        }
    }

    eat(); // Eat the )

    if (check(TK_LBRACE)) {
        returnTypes.push_back(newVoid());
    } else if (match(TK_COLON)) {
        // expectAfter(TK_COLON, ":", ")");
        do {
            returnTypes.push_back(parseType());
        } while (match(TK_COMMA));
    } else {
        parseError(":", ")");
    }

    return newPrototype(name, args, returnTypes);
}

fn Parser::parseFunction() -> Node* {
    eat(); // Eat the fn
    _inFunction = true;

    if (!check(TK_IDENT)) {
        return parseError("name", "fn");
    }

    String name = _current.lexeme;
    _currentFnName = name;

    expectAfter(TK_LPAREN, "(", "name");
    eat(); // eat the (
    PrototypeNode *prototype = parsePrototype(name);
    BlockNode *body = parseBlock();

    return newFunction(prototype, body);
}

fn Parser::parseReturn() -> Node* {
    eat();
    
    if (!_inFunction) {
        return parseErr("A return statement must be within a function.");
    }
    
    Nodes retStatements;
    while (_current.type != TK_SEMICOLON && _current.type != TK_RBRACE) {
        let *retStatement = parseExpression();
        retStatements.push_back(retStatement);
        if (_current.type != TK_COMMA || _current.type == TK_EOF) {
            break;
        }
        eat();
    }
    
    maybeSemicolon();
    return newReturn(retStatements);
}
