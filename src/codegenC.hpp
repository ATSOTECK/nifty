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

#ifndef __NIFTY_CODEGENC_HPP__
#define __NIFTY_CODEGENC_HPP__

#include "common.hpp"
#include "node.hpp"

#include <fstream>

class CodegenC {
public:
    fn generate(const Nodes &ast) -> void; // TODO: Use a package obj.
    
private:
    fn insertNL(int count = 1) -> void;
    fn insertTab(int count = 1) -> void;
    fn insertTimestamp() -> void;
    fn insertBuiltinTypes() -> void;
    
    fn insertMainFunctionDef() -> void;
    
    fn codegen(Node *node) -> void;
    fn genNumberType(NumberType *numberType) -> void;
    fn genType(NiftyType *niftyType) -> void;
    fn genPrototype(PrototypeNode *prototype, const String &rename = "") -> void;
    fn genBlock(BlockNode *block, int level = 0) -> void;
    fn genFunction(FunctionNode *function) -> void;
    fn genReturn(ReturnNode *returnNode) -> void;
    
    String _package;
    std::fstream _file;
};


#endif //__NIFTY_CODEGENC_HPP__
