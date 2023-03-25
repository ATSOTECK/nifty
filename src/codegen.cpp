//
// Created by Skyler on 2/21/23.
//

#include "codegen.hpp"

fn Codegen::generate(Node *ast) {
    let ir = genCode(ast);
    let ret = _builder->CreateIntCast(ir, _builder->getInt32Ty(), true);
    _builder->CreateRet(ret);
}

fn Codegen::saveIRToFile() {
    std::error_code errorCode;
    llvm::raw_fd_ostream out("nifty.ll", errorCode);
    _modules[0]->print(out, nullptr);
}

fn Codegen::initTarget() {
    //
}

fn Codegen::initModules() {
    _ctx = std::make_unique<llvm::LLVMContext>();
    let defaultModule = std::make_unique<llvm::Module>("default", *_ctx);
    _modules.push_back(std::move(defaultModule));
    _builder = std::make_unique<llvm::IRBuilder<>>(*_ctx);
}

fn Codegen::genCode(Node *ast) -> llvm::Value* {
    return _builder->getInt32(42);
}
/*
fn Codegen::genPrototype(PrototypeNode *prototype) -> llvm::Function {
    //
}

fn Codegen::genFunction(FunctionNode *function) -> llvm::Function* {
    //
}
 */
