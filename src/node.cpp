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

#include "node.hpp"

fn newVoid() -> Node* {
    let node = new VoidNode{};
    node->node.type = VoidNodeType;
    return recast(node, Node*);
}

fn newType(const String &typeStr) -> Node* {
    let node = new TypeNode{};
    node->node.type = TypeNodeType;
    node->typeStr = typeStr;
    return recast(node, Node*);
}

fn newInt(int bits, const String &value, bool isSigned) -> Node* {
    let node = new IntNode{};
    node->node.type = IntNodeType;
    node->bits = bits;
    node->value = value;
    node->isSigned = isSigned;
    return recast(node, Node*);
}

fn newFloat(int bits, const String &value) -> Node* {
    let node = new FloatNode{};
    node->node.type = FloatNodeType;
    node->bits = bits;
    node->value = value;
    return recast(node, Node*);
}

fn newBool(int bits, bool value) -> Node* {
    let node = new BoolNode{};
    node->node.type = BoolNodeType;
    node->bits = bits;
    node->value = value;
    return recast(node, Node*);
}

fn newString(bool isCString, const String &value) -> Node* {
    let node = new StringNode{};
    node->node.type = StringNodeType;
    node->isCString = isCString;
    node->value = value;
    return recast(node, Node*);
}

fn newChar(uint32 value) -> Node* {
    let node = new CharNode{};
    node->node.type = CharNodeType;
    node->value = value;
    return recast(node, Node*);
}

fn newBinary(const Token &op, Node *lhs, Node *rhs) -> Node* {
    let node = new BinaryNode{};
    node->node.type = BinaryNodeType;
    node->op = op;
    node->lhs = lhs;
    node->rhs = rhs;
    return recast(node, Node*);
}

fn newCall(const String &calle, Nodes args) -> Node* {
    let node = new CallNode{};
    node->node.type = CallNodeType;
    node->callee = calle;
    node->args = std::move(args);
    return recast(node, Node*);
}

fn newPrototype(const String &name, Nodes args, Nodes returnTypes) -> Node* {
    let node = new PrototypeNode{};
    node->node.type = PrototypeNodeType;
    node->name = name;
    node->args = std::move(args);
    node->returnTypes = std::move(returnTypes);
    return recast(node, Node*);
}

fn newBlock(Nodes statements) -> Node* {
    let node = new BlockNode{};
    node->node.type = BlockNodeType;
    node->statements = std::move(statements);
    return recast(node, Node*);
}

fn newNamedBlock(const String &name, Nodes statements) -> Node* {
    let node = new NamedBlockNode{};
    node->node.type = NamedBlockNodeType;
    node->name = name;
    node->statements = std::move(statements);
    return recast(node, Node*);
}

fn newFunction(PrototypeNode *prototype, BlockNode *body) -> Node* {
    let node = new FunctionNode{};
    node->node.type = FunctionNodeType;
    node->prototype = prototype;
    node->body = body;
    return recast(node, Node*);
}

fn newReturn(Nodes statements) -> Node* {
    let node = new ReturnNode{};
    node->node.type = ReturnNodeType;
    node->statements = std::move(statements);
    return recast(node, Node*);
}

fn newVar(const String &name, Node *type, Node *value, VarKind kind) -> Node* {
    let node = new VarNode{};
    node->node.type = VarNodeType;
    node->name = name;
    node->type = type;
    node->value = value;
    node->kind = kind;
    return recast(node, Node*);
}

