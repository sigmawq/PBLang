//
// Created by foxfurry on 6/1/21.
//

#include "ExprIR.h"

llvm::Type *str_to_type(std::string str_ty){
    if(str_ty=="int")return llvm::Type::getInt32Ty(*llvm_context);
    else if(str_ty=="double")return llvm::Type::getFloatTy(*llvm_context);
    else if(str_ty=="float")return llvm::Type::getDoubleTy(*llvm_context);
    else{
        return nullptr;
    }
}


std::unique_ptr<ExprIR> LogError(const char *Str) {
    fprintf(stderr, "Error: %s\n", Str);
    return nullptr;
}

llvm::Value *LogErrorV(const char *Str) {
    LogError(Str);
    return nullptr;
}
