//
// Created by foxfurry on 6/1/21.
//

#include "FuncIR.h"

llvm::Function *FuncIR::codegen() {
    // First, check for an existing function from a previous 'extern' declaration.
    llvm::Function *TheFunction = llvm_module->getFunction(proto->getName());

    if (!TheFunction)
        TheFunction = proto->codegen();

    if (!TheFunction)
        return nullptr;

    // Create a new basic block to start insertion into.
    llvm::BasicBlock *BB = llvm::BasicBlock::Create(*llvm_context, "entry", TheFunction);
    llvm_builder->SetInsertPoint(BB);

    // Record the function arguments in the NamedValues map.
    named_values.clear();
    for (auto &Arg : TheFunction->args())
        named_values[std::string(Arg.getName())] = &Arg;

    if (llvm::Value *RetVal = body->codegen()) {
        // Finish off the function.
        llvm_builder->CreateRet(RetVal);

        // Validate the generated code, checking for consistency.
        verifyFunction(*TheFunction);

        return TheFunction;
    }

    // Error reading body, remove function.
    TheFunction->eraseFromParent();
    return nullptr;
}