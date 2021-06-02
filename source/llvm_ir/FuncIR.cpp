//
// Created by foxfurry on 6/1/21.
//

#include "FuncIR.h"

llvm::Function *FuncIR::codegen() {
    // First, check for an existing function from a previous 'extern' declaration.
    llvm::Function *TheFunction = llvm_module->getFunction(proto->getName());
    //;

    if (!TheFunction)
        TheFunction = proto->codegen(node);

    if (!TheFunction)
        return nullptr;

    named_values.clear();
    for (auto &Arg : TheFunction->args())
        named_values[std::string(Arg.getName())] = &Arg;

    return TheFunction;
}