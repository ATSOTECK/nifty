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

#ifndef __NIFTY_NODE_HPP__
#define __NIFTY_NODE_HPP__

#include <utility>
#include <vector>

#include "token.hpp"
#include "util/str.hpp"

enum NodeType {
    EmptyNodeType,
    TypeNodeType,
    VoidNodeType,
    IntNodeType,
    FloatNodeType,
    BoolNodeType,
    StringNodeType,
    CharNodeType,
    BinaryNodeType,
    CallNodeType,
    PrototypeNodeType,
    BlockNodeType,
    NamedBlockNodeType,
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
    Node() = delete;
};

typedef std::vector<Node*> Nodes;

struct VoidNode {
    Node node;
};

struct TypeNode {
    Node node;
    String typeStr;
};

struct IntNode {
    Node node;
    int bits;
    String value;
    bool isSigned;
};

struct FloatNode {
    Node node;
    int bits;
    String value;
};

struct BoolNode {
    Node node;
    int bits;
    bool value;
};

struct StringNode {
    Node node;
    bool isCString;
    String value;
};

struct CharNode {
   Node node;
   uint32 value;
};

struct BinaryNode {
    Node node;
    Token op;
    Node *lhs, *rhs;
};

struct CallNode {
    Node node;
    String callee;
    Nodes args;
};

struct PrototypeNode {
    Node node;
    String name;
    Nodes args;
    Nodes returnTypes;
};

struct BlockNode {
    Node node;
    Nodes statements;
};

struct NamedBlockNode {
    Node node;
    String name;
    Nodes statements;
};

struct FunctionNode {
    Node node;
    PrototypeNode *prototype;
    BlockNode *body;
};

struct ReturnNode {
    Node node;
    Node *statement;
};

struct VarNode {
    Node node;
    String name;
    Node *type;
    Node *value; // If null, then undefined.
    VarKind kind;
};

Node *newVoid();
Node *newType(const String &typeStr);
Node *newInt(int bits, const String &value, bool isSigned);
Node *newFloat(int bits, const String &value);
Node *newBool(int bits, bool value);
Node *newString(bool isCString, const String &value);
Node *newChar(uint32 value);
Node *newBinary(const Token &op, Node *lhs, Node *rhs);
Node *newCall(const String &calle, Nodes args);
Node *newPrototype(const String &name, Nodes args, Nodes returnTypes);
Node *newBlock(Nodes statements);
Node *newNamedBlock(const String &name, Nodes statements);
Node *newFunction(PrototypeNode *prototype, BlockNode *body);
Node *newReturn(Node *statement);
Node *newVar(const String &name, Node *type, Node *value, VarKind kind);

#endif //__NIFTY_NODE_HPP__
