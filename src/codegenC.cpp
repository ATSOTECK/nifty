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

#include "codegenC.hpp"

fn CodegenC::generate(const Nodes &ast) -> void {
    _file.open(NIFTY_GENERATED_FILE, std::ofstream::out | std::ofstream::trunc);
    
    _file << "//\n";
    _file << "//  " << NIFTY_GENERATED_FILE << "\n";
    _file << "//\n";
    _file << "//  Generated by the Nifty compiler v " << NIFTY_VERSION <<" on ";
    insertTimestamp();
    _file << "//  !!! All changes made to this file will be overwritten. !!!\n";
    _file << "//";
    
    insertNL(2);
    insertBuiltinTypes();
    insertNL();
    
    _file << "int " << NIFTY_ENTRY << "();";
    insertNL(2);
    
    // TODO: Loop over the ast and insert any multi-return structs that are needed.
    // TODO: Have a way to keep track of what was already inserted.
    // Should all types be generated before functions?
    
    for (const let &node : ast) {
        codegen(node);
    }
    
    _file << "int main(void) {\n";
    _file << "\treturn " << NIFTY_ENTRY << "();\n";
    _file << "}\n";
}

fn CodegenC::insertNL(int count) -> void {
    for (int i = 0; i < count; ++i) {
        _file << std::endl;
    }
}

fn CodegenC::insertTab(int count) -> void {
    if (count == 0) {
        return;
    }
    
    for (int i = 0; i < count; ++i) {
        _file << "    ";
    }
}

fn CodegenC::insertTimestamp() -> void {
    time_t t = time(nullptr);
    struct tm *now = localtime(&t);
    _file << (now->tm_mon + 1) << "/";
    _file << (now->tm_mday) << "/";
    _file << (now->tm_year + 1900) << ".\n";
}

fn CodegenC::insertBuiltinTypes() -> void {
    _file << "#include <inttypes>\n";
    _file << "#ifndef __cplusplus\n";
    _file << "#\tinclude <stdbool>\n";
    _file << "#endif\n";
    _file << "\n";
    _file << "typedef uint8_t nifty_u8;\n";
    _file << "typedef uint16_t nifty_u16;\n";
    _file << "typedef uint32_t nifty_u32;\n";
    _file << "typedef uint32_t nifty_uint;\n";
    _file << "typedef uint64_t nifty_u64;\n";
    _file << "typedef int8_t nifty_s8;\n";
    _file << "typedef int16_t nifty_s16;\n";
    _file << "typedef int32_t nifty_s32;\n";
    _file << "typedef int64_t nifty_s64;\n";
    _file << "typedef float nifty_f32;\n";
    _file << "typedef double nifty_f64;\n";
    _file << "typedef char* nifty_cstring;\n";
    _file << "typedef void* nifty_rawptr;\n";
    _file << "typedef uint32_t nifty_char;\n";
    _file << "typedef uint8_t nifty_b8;\n";
    _file << "typedef uint16_t nifty_b16;\n";
    _file << "typedef uint32_t nifty_b32;\n";
    _file << "typedef uint64_t nifty_b64;\n";
}

fn CodegenC::codegen(Node *node) -> void {
    switch (node->type) {
        case FunctionNodeType: genFunction(recast(node, FunctionNode*)); break;
        case ReturnNodeType: genReturn(recast(node, ReturnNode*)); break;
        default: db("Not yet implemented."); break;
    }
}

fn CodegenC::genNumberType(NumberType *numberType) -> void {
    switch (numberType->numberKind) {
        case NumberTypeKind::S32: _file << "int"; break;
        default: db("Number type not yet implemented."); break;
    }
}

fn CodegenC::genType(NiftyType *niftyType) -> void {
    switch (niftyType->kind) {
        case VoidKind: _file << "void"; break;
        case NumberKind: genNumberType(recast(niftyType, NumberType*)); break;
        default: db("Type not yet supported."); break;
    }
}

fn CodegenC::genPrototype(PrototypeNode *prototype, const String &rename) -> void {
    if (prototype->returnTypes.size() > 1) {
        db("Nifty does not yet support more than one return type.");
        return;
    }
    
    genType(prototype->returnTypes[0]);
    if (rename.empty()) {
        _file << " " << prototype->name << "("; // TODO: Use package name as well.
    } else {
        _file << " " << rename << "(";
    }
    
    for (size_t i = 0; i < prototype->args.size(); ++i) {
        let arg = prototype->args[i];
        genType(arg->type);
        _file << " " << arg->name;
        
        if (i != prototype->args.size() - 1) {
            _file << ", ";
        }
    }
    
    _file << ")";
}

fn CodegenC::genBlock(BlockNode *block, int level) -> void {
    _file << "{\n";
    for (const let statement : block->statements) {
        insertTab(level + 1);
        codegen(statement);
    }
    _file << "}\n";
}

fn CodegenC::genFunction(FunctionNode *function) -> void {
    if (function->isEntry) {
        genPrototype(function->prototype, NIFTY_ENTRY);
    } else {
        genPrototype(function->prototype);
    }
    _file << " ";
    genBlock(function->body);
    insertNL();
}

fn CodegenC::genReturn(ReturnNode *returnNode) -> void {
    _file << "return";
    if (returnNode->statements.empty()) {
        _file << ";";
    } else {
        _file << " ";
        codegen(recast(returnNode->statements[0], Node*));
    }
}
