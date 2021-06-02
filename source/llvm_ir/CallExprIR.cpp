//
// Created by foxfurry on 6/1/21.
//

#include "CallExprIR.h"

llvm::Value *CallExprIR::codegen() {
    // Look up the func_name in the global module table.
    llvm::Function *CalleeF = llvm_module->getFunction(callee);
    if (!CalleeF)
        return LogErrorV("Unknown function referenced");

    // If argument mismatch error.
    if (CalleeF->arg_size() != args.size())
        return LogErrorV("Incorrect # arguments passed");

    std::vector<llvm::Value *> ArgsV;
    for (unsigned i = 0, e = args.size(); i != e; ++i) {
        ArgsV.push_back(args[i]->codegen());
        if (!ArgsV.back())
            return nullptr;
    }

    return llvm_builder->CreateCall(CalleeF, ArgsV, "calltmp");
}
