//
// Created by foxfurry on 6/1/21.
//

#include "BinaryExprIR.h"

llvm::Value *BinaryExprIR::codegen() {
    llvm::Value *L = LHS->codegen(),
                *R = RHS->codegen();

    if(!L || !R){
        return nullptr;
    }
    switch (op) {
        case ADD:
            return llvm_builder->CreateFAdd(L, R, "addtmp");
        case SUB:
            return llvm_builder->CreateFSub(L, R, "subtmp");
        case MUL:
            return llvm_builder->CreateFMul(L, R, "multmp");
        case DIV:
            return llvm_builder->CreateFDiv(L, R, "addtmp");
        case COMP_LESS:
            L = llvm_builder->CreateFCmpULT(L,R,"cmptmp");
            return llvm_builder->CreateUIToFP(L, llvm::Type::getDoubleTy(*llvm_context), "cmptmp");
        default:
            return LogErrorV("Invalid binary operator");
    }
}

