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

#ifndef __NIFTY_NODE_HPP__
#define __NIFTY_NODE_HPP__

#include <utility>
#include <vector>

#include "token.hpp"
#include "util/str.hpp"

enum NodeType {
    EmptyNodeType,
    TypeNodeType,
    IntNodeType,
    FloatNodeType,
    BoolNodeType,
    StringNodeType,
    CharNodeType,
    BinaryNodeType,
    CallNodeType,
    PrototypeNodeType,
    BlockNodeType,
    FunctionNodeType,
    ReturnNodeType,
    VarNodeType,
};

enum VarKind {
    Let,
    Val,
    Const,
};

struct Node {
    NodeType type;
};

struct TypeNode : Node {
    TypeNode(String lexeme) : Node(),
        strType(std::move(lexeme))
    {
        type = TypeNodeType;
    }

    String strType;
};

struct IntNode : Node {
    IntNode(int b, String v, bool s) : Node(),
        bits(b),
        value(std::move(v)),
        isSigned(s)
    {
        type = IntNodeType;
    }

    int bits;
    String value;
    bool isSigned;
};

struct FloatNode : Node {
    FloatNode(int b, String v, bool s) : Node(),
        bits(b),
        value(std::move(v)),
        isSigned(s)
    {
        type = FloatNodeType;
    }

    int bits;
    String value;
    bool isSigned;
};

struct BoolNode : Node {
    BoolNode(bool v) : Node(),
        value(v)
    {
        type = BoolNodeType;
    }

    bool value;
};

struct StringNode : Node {
    StringNode(String s) : Node(),
        str(std::move(s))
    {
        type = StringNodeType;
    }

    String str;
};

struct CharNode : Node {
    CharNode(char c) : Node(),
        c(c)
    {
        type = CharNodeType;
    }

    char c;
};

struct BinaryNode : Node {
    BinaryNode(const Token &op, Node *lhs, Node *rhs) : Node(),
        op(op),
        lhs(lhs),
        rhs(rhs)
    {
        type = BinaryNodeType;
    }

    Token op;
    Node *lhs;
    Node *rhs;
};

struct CallNode : Node {
    CallNode(String calle, std::vector<Node*> args) : Node(),
        calle(std::move(calle)),
        args(std::move(args))
    {
        type = CallNodeType;
    }

    String calle;
    std::vector<Node*> args;
};

struct PrototypeNode : Node {
    PrototypeNode(String name, std::vector<Node*> args, std::vector<Node*> returnTypes) : Node(),
        name(std::move(name)),
        args(std::move(args)),
        returnTypes(std::move(returnTypes))
    {
        type = PrototypeNodeType;
    }

    String name;
    std::vector<Node*> args;
    std::vector<Node*> returnTypes;
};

struct BlockNode : Node {
    BlockNode() : Node() {
        type = BlockNodeType;
    }

    std::vector<Node*> statements;
};

struct FunctionNode : Node {
    FunctionNode(PrototypeNode *proto, BlockNode *body) : Node(),
        prototype(proto),
        body(body)
    {
        type = FunctionNodeType;
    }

    PrototypeNode *prototype;
    BlockNode *body;
};

struct ReturnNode : Node {
    ReturnNode(Node *returnStatement) : Node(),
        returnStatement(returnStatement)
    {
        type = ReturnNodeType;
    }

    Node *returnStatement;
};

struct VarNode : Node {
    VarNode(String name, Node *varType, Node *value, VarKind kind, bool undefined) : Node(),
        name(std::move(name)),
        varType(varType),
        value(value),
        kind(kind),
        undefined(undefined)
    {
        type = VarNodeType;
    }

    String name;
    Node *varType;
    Node *value;
    VarKind kind;
    bool undefined;
};

#endif //__NIFTY_NODE_HPP__
