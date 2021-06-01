//
// Created by foxfurry on 6/1/21.
//

#include "ExprIR.h"

std::unique_ptr<ExprIR> LogError(const char *Str) {
    fprintf(stderr, "Error: %s\n", Str);
    return nullptr;
}

llvm::Value *LogErrorV(const char *Str) {
    LogError(Str);
    return nullptr;
}
