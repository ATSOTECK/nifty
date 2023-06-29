//
// Created by Skyler on 2/21/23.
//

#include "codegen.hpp"
/*

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

fn Codegen::genPrototype(PrototypeNode *prototype) -> llvm::Function* {
    std::vector<llvm::Type*> arguments(prototype->args.size());
    for (const let arg : prototype->args) {
        arguments.push_back(toLLVMType(arg->type));
    }
    
    let returnType = toLLVMType(prototype->returnTypes[0]); // TODO: Multiple return types.
    let functionType = llvm::FunctionType::get(returnType, arguments, prototype->hasVarargs);
    let functionName = prototype->name.stdString();
    let linkage = prototype->isExtern || functionName == "main" ? llvm::Function::ExternalLinkage : llvm::Function::InternalLinkage;
    let function = llvm::Function::Create(functionType, linkage, functionName, _module.get());
    
    int index = 0;
    for (let &arg : function->args()) {
        if (index >= prototype->args.size()) {
            break;
        }
        arg.setName(prototype->args[index++]->name.stdString());
    }
    
    return function;
}

fn Codegen::genReturn(ReturnNode *ret) -> llvm::ReturnInst* {
    // TODO: Emit any defer statements before return statements.
    int returnCount = (int)ret->statements.size();
    
    if (returnCount == 0) {
        return _builder->CreateRetVoid();
    }
}

fn Codegen::genInt(IntNode *intNode) -> llvm::ConstantInt* {
    llvm::IntegerType *intType = llvm::IntegerType::get(*_ctx, intNode->bits); // TODO: Do I need to remake this every time?
    let value = std::stoll(intNode->value.c_str(), nullptr, 10); // TODO: Support hex/oct/bin
    return llvm::ConstantInt::get(intType, value, intNode->isSigned);
}

fn Codegen::toLLVMType(NiftyType *niftyType) -> llvm::Type* {
    TypeKind kind = niftyType->kind;
    if (kind == NumberKind) {
        let numberType = recast(niftyType, NumberType*);
        NumberTypeKind numberKind = numberType->numberKind;
        switch (numberKind) {
            case NumberTypeKind::I1: return llvm::Type::getInt1Ty(*_ctx);
            case NumberTypeKind::S8:
            case NumberTypeKind::U8: return llvm::Type::getInt8Ty(*_ctx);
            case NumberTypeKind::S16:
            case NumberTypeKind::U16: return llvm::Type::getInt16Ty(*_ctx);
            case NumberTypeKind::S32:
            case NumberTypeKind::U32: return llvm::Type::getInt32Ty(*_ctx);
            case NumberTypeKind::S64:
            case NumberTypeKind::U64: return llvm::Type::getInt64Ty(*_ctx);
            case NumberTypeKind::S128:
            case NumberTypeKind::U128: return llvm::Type::getInt128Ty(*_ctx);
            case NumberTypeKind::F16: return llvm::Type::getHalfTy(*_ctx);
            case NumberTypeKind::F32: return llvm::Type::getFloatTy(*_ctx);
            case NumberTypeKind::F64: return llvm::Type::getDoubleTy(*_ctx);
            case NumberTypeKind::F128: return llvm::Type::getFP128Ty(*_ctx);
        }
    } else if (kind == VoidKind) {
        return llvm::Type::getVoidTy(*_ctx);
    } else if (kind == PointerKind) {
        let pointerType = recast(niftyType, PointerType*);
        if (pointerType->pointerBaseType->kind == VoidKind) {
            // rawptr
            return llvm::Type::getInt8PtrTy(*_ctx);
        }
        let llvmPointerType = toLLVMType(pointerType->pointerBaseType);
        return llvm::PointerType::get(llvmPointerType, 0);
    }
    return nullptr;
}
 
*/

/*
fn Codegen::genPrototype(PrototypeNode *prototype) -> llvm::Function {
    //
}

fn Codegen::genFunction(FunctionNode *function) -> llvm::Function* {
    //
}
 */
