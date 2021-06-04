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
        case COMP_GREATER:
            L = llvm_builder->CreateFCmpULT(R,L,"cmptmp");
            return llvm_builder->CreateUIToFP(L, llvm::Type::getDoubleTy(*llvm_context), "cmptmp");
        case LOGICAL_OR:
            return llvm_builder->CreateOr(L,R, "ortmp");
        case LOGICAL_AND:
            return llvm_builder->CreateAnd(L,R,"andtmp");
        default:
            return LogErrorV("Invalid binary operator");
    }
}

