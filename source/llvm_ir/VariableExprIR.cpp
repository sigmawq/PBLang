//
// Created by foxfurry on 6/3/21.
//

#include "VariableExprIR.h"

llvm::Value *VariableExprIR::codegen(){
    llvm::Function *F = llvm_builder->GetInsertBlock()->getParent();

    llvm::Value *init_val;

    if(body){
        init_val = body->codegen();
        if(!init_val)
            return nullptr;
    }
    else{
        init_val = llvm::ConstantFP::get(*llvm_context, llvm::APFloat(0.0));
    }

    llvm::AllocaInst *Alloca = CreateEntryBlockAlloca(F, var_name);
    llvm_builder->CreateStore(init_val, Alloca);

    named_values[var_name] = Alloca;

    return init_val;
}
