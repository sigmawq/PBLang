//
// Created by foxfurry on 6/1/21.
//

#include "BinaryExprIR.h"

llvm::Value *BinaryExprIR::codegen() {
    llvm::Value *L = LHS->codegen(),
                *R = RHS->codegen();

    if(!L || !R){
        return LogErrorV("LHS or RHS not evaluated");
    }
    auto LType = L->getType(),
         RType = R->getType();

    if(LType->isPointerTy() || RType->isPointerTy()){
        return LogErrorV("Strings don't support binary operations");
    }
    else if(LType->isFloatTy() || RType->isFloatTy()){
        L = llvm_builder->CreateSIToFP(L, llvm::Type::getFloatTy(*llvm_context), "SItoFP");
        R = llvm_builder->CreateSIToFP(R, llvm::Type::getFloatTy(*llvm_context), "SItoFP");

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
                return llvm_builder->CreateUIToFP(L, llvm::Type::getFloatTy(*llvm_context), "cmptmp");
            case COMP_GREATER:
                L = llvm_builder->CreateFCmpULT(R,L,"cmptmp");
                return llvm_builder->CreateUIToFP(L, llvm::Type::getFloatTy(*llvm_context), "cmptmp");
            case EQUALS:
                return llvm_builder->CreateStore(R, named_values[L->getName().str()]);
            default:
                return LogErrorV("Invalid binary operator");
        }
    }
    switch (op) {
        case ADD:
            return llvm_builder->CreateAdd(L, R, "addtmp");
        case SUB:
            return llvm_builder->CreateSub(L, R, "subtmp");
        case MUL:
            return llvm_builder->CreateMul(L, R, "multmp");
        case DIV:
            return llvm_builder->CreateSDiv(L, R, "addtmp");
        case COMP_LESS:
            return llvm_builder->CreateICmpSLT(L,R,"cmptmp");
        case COMP_GREATER:
            return llvm_builder->CreateICmpSLT(R,L,"cmptmp");
        case LOGICAL_OR:
            return llvm_builder->CreateOr(L,R, "ortmp");
        case LOGICAL_AND:
            return llvm_builder->CreateAnd(L,R,"andtmp");
        case EQUALS:
            return llvm_builder->CreateStore(R, named_values[L->getName().str()]);
        default:
            return LogErrorV("Invalid binary operator");
    }

}

