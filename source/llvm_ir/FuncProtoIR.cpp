//
// Created by foxfurry on 6/1/21.
//

#include "FuncProtoIR.h"

llvm::Function *FuncProtoIR::codegen() {
    // Make the function type:  double(double,double) etc.
    std::vector<llvm::Type *> Doubles(args.size(), llvm::Type::getDoubleTy(*llvm_context));
    llvm::FunctionType *FT =
            llvm::FunctionType::get(llvm::Type::getDoubleTy(*llvm_context), Doubles, false);

    llvm::Function *F =
            llvm::Function::Create(FT,
                                   llvm::Function::ExternalLinkage,
                                   name,
                                   llvm_module.get());

    // Set names for all arguments.
    unsigned Idx = 0;
    for (auto &Arg : F->args())
        Arg.setName(args[Idx++]);

    return F;
}

std::unique_ptr<FuncProtoIR> LogErrorP(const char *Str) {
    LogError(Str);
    return nullptr;
}