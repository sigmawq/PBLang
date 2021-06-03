//
// Created by foxfurry on 6/1/21.
//

#ifndef LEXER_TEST_FUNCPROTOIR_H
#define LEXER_TEST_FUNCPROTOIR_H

#include "ExprIR.h"

class FuncProtoIR{
    std::string func_name = "";
    std::vector<std::string> args_name;

public:
    FuncProtoIR(const std::string &_name, std::vector<std::string> _args)
            : func_name(_name), args_name(std::move(_args)) {}
    FuncProtoIR(){}
    llvm::Function *codegen(std::shared_ptr<ast_node> node);
    const std::string &getName() const { return func_name; }
};

std::unique_ptr<FuncProtoIR> LogErrorP(const char *Str);

#endif //LEXER_TEST_FUNCPROTOIR_H
