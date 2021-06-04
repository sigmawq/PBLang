//
// Created by foxfurry on 6/1/21.
//

#include "NumberExprIR.h"

llvm::Value *NumberExprIR::codegen() {
    if(((int)val) == val){
        return llvm::ConstantInt::get(*llvm_context, llvm::APInt(32, (int)val, true));
    }
    return llvm::ConstantFP::get(*llvm_context, llvm::APFloat(val));
}
