//
// Created by foxfurry on 6/4/21.
//

#include "IfExprIR.h"

llvm::Value * IfExprIR::codegen(){
    llvm::Value *cond_value = if_cond->codegen();

    if(!cond_value){
        return nullptr;
    }

    cond_value = llvm_builder->CreateFCmpONE(cond_value, llvm::ConstantFP::get(*llvm_context, llvm::APFloat(0.0)), "ifcond");


}