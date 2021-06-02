//
// Created by foxfurry on 5/24/21.
//

#ifndef LEXER_TEST_LLVM_IR_H
#define LEXER_TEST_LLVM_IR_H

#include "FuncIR.h"
#include "BinaryExprIR.h"
#include "CallExprIR.h"
#include "NumberExprIR.h"
#include "IdentifierExprIR.h"

llvm::Value* stmt_to_ir(std::shared_ptr<ast_node> &stmt);
std::unique_ptr<ExprIR> expr_to_ir(std::shared_ptr<ast_node> &expr_node);
std::unique_ptr<FuncIR> func_to_ir(std::shared_ptr<ast_node> &func_node);

llvm::Value* stmt_to_ir(std::shared_ptr<ast_node> &stmt){
    if(stmt->node_type != STATEMENT_SEQUENCE)return nullptr;

    for(auto child: stmt->children){
        if(child->node_type == RETURN_STMT){
            return expr_to_ir(child->children[0])->codegen();
        }
        else if(child->node_type == F_DECL){
            auto func = func_to_ir(child)->codegen();
            auto body = child->get_child(STATEMENT_SEQUENCE);

            llvm::BasicBlock *BB = llvm::BasicBlock::Create(*llvm_context, func->getName(), func);
            llvm_builder->SetInsertPoint(BB);

            auto return_value = stmt_to_ir(body);
            llvm_builder->SetInsertPoint(BB);
            llvm_builder->CreateRet(return_value);

            llvm::verifyFunction(*func);
        }
        else {
            expr_to_ir(child)->codegen();
        }

    }
    return nullptr;
}

std::unique_ptr<ExprIR> expr_to_ir(std::shared_ptr<ast_node> &expr_node){
    if(expr_node->is_operator()){
        auto binTmp = std::make_unique<BinaryExprIR>(expr_node);
        binTmp->LHS = expr_to_ir(expr_node->children[0]);
        binTmp->RHS = expr_to_ir(expr_node->children[1]);
        return binTmp;
    }
    else if(expr_node->node_type == VAL_INT || expr_node->node_type == VAL_FP){
        return std::make_unique<NumberExprIR>(expr_node);
    }
    else if(expr_node->node_type == IDENTIFIER_AST){
        return std::make_unique<IdentifierExprIR>(expr_node);
    }

}

std::unique_ptr<FuncIR> func_to_ir(std::shared_ptr<ast_node> &func_node){
    return std::make_unique<FuncIR>(func_node);
}

void generate_ir(std::shared_ptr<ast_node> &root) {
    llvm_context = std::make_unique<llvm::LLVMContext>();
    llvm_module =  std::make_unique<llvm::Module>     (JIT_MODULE_NAME, *llvm_context);
    llvm_builder = std::make_unique<llvm::IRBuilder<>>(llvm_module->getContext());

    stmt_to_ir(root);

#ifdef DEBUG_MODE
    llvm_module->print(llvm::errs(), nullptr, false, true);
#endif
}



#endif //LEXER_TEST_LLVM_IR_H
