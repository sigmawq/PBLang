//
// Created by foxfurry on 6/1/21.
//

#include "IdentifierExprIR.h"

llvm::Value *IdentifierExprIR::codegen() {
    llvm::Value *V = named_values[name];
    if(!V){
        return LogErrorV("Unknown variable func_name");
    }
    return V;
}
