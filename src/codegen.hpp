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

#ifndef NIFTY_CODEGEN_HPP
#define NIFTY_CODEGEN_HPP

#include "common.hpp"
#include "node.hpp"

#include <memory>
#include <vector>

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Type.h>

class Codegen {
public:
    fn generate(Node *ast);

    fn saveIRToFile();
private:
    fn initTarget();
    fn initModules();

    fn genCode(Node *ast) -> llvm::Value*;
    fn genPrototype(PrototypeNode *prototype) -> llvm::Function*;
    fn genFunction(FunctionNode *function) -> llvm::Function*;
    fn genBlock(BlockNode *block);
    
    fn toLLVMType(NiftyType *niftyType) -> llvm::Type*;

    std::unique_ptr<llvm::LLVMContext> _ctx;
    std::vector<std::unique_ptr<llvm::Module>> _modules;
    std::unique_ptr<llvm::IRBuilder<>> _builder;
};


#endif //NIFTY_CODEGEN_HPP
