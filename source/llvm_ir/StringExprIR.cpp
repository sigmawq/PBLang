//
// Created by foxfurry on 6/4/21.
//

#include "StringExprIR.h"

llvm::Value *StringExprIR::codegen() {
    return llvm_builder->CreateGlobalStringPtr(value);

}