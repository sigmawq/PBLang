//
// Created by foxfurry on 6/1/21.
//

#include "IdentifierExprIR.h"

llvm::Value *IdentifierExprIR::codegen() {
    llvm::Value *V = nullptr;
    if(!isFuncCall){
        V = named_values[name];
        if (!V) {
            return LogErrorV("Unknown variable func_name");
        }
    }
    else{
        llvm::Function *F = llvm_module->getFunction(name);

        if(!F){
            return LogErrorV("Unknown function referenced");
        }
        if(F->arg_size() != args.size()){
            return LogErrorV("Incorrect # arguments passed");
        }

        std::vector<llvm::Value *> args_val;
        for(unsigned idx = 0; idx != args.size(); idx++){
            args_val.push_back(args[idx]->codegen());
            if(!args_val.back()){
                return nullptr;
            }
        }
        V = llvm_builder->CreateCall(F, args_val, "calltmp");
    }
    return V;
}
