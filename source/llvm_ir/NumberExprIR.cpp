//
// Created by foxfurry on 6/1/21.
//

#include "NumberExprIR.h"

llvm::Value *NumberExprIR::codegen() {
    return llvm::ConstantFP::get(*llvm_context, llvm::APFloat(val));
}
