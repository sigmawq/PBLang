//
// Created by foxfurry on 6/1/21.
//

#include "VariableExprIR.h"

llvm::Value *VariableExprIR::codegen() {
    llvm::Value *V = named_values[name];
    if(!V){
        return LogErrorV("Unknown variable name");
    }
    return V;
}
