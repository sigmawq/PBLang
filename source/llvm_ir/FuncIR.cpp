//
// Created by foxfurry on 6/1/21.
//

#include "FuncIR.h"

llvm::Function *FuncIR::codegen() {
    // First, check for an existing function from a previous 'extern' declaration.
    llvm::Function *function_obj = llvm_module->getFunction(proto->getName());
    //;

    if (!function_obj)
        function_obj = proto->codegen(node);

    if (!function_obj)
        return nullptr;



    return function_obj;
}