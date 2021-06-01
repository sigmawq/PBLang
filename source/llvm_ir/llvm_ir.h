//
// Created by foxfurry on 5/24/21.
//

#ifndef LEXER_TEST_LLVM_IR_H
#define LEXER_TEST_LLVM_IR_H

#include "FuncIR.h"
#include "BinaryExprIR.h"
#include "CallExprIR.h"
#include "NumberExprIR.h"
#include "VariableExprIR.h"


std::unique_ptr<ExprIR> node_to_ir(std::shared_ptr<ast_node> &node){
    switch(node->node_type){
        case F_DECL:
            FuncIR(node).codegen();
            return nullptr;
//        case F_CALL:
//            return std::make_unique<CallExprIR>(node);

        case STATEMENT_SEQUENCE:
            llvm::BasicBlock *BB = llvm::BasicBlock::Create(*llvm_context,"sequence entry");
            llvm_builder->SetInsertPoint(BB);

            llvm::Value *return_node = nullptr;
            for (auto child: node->children) {
                auto tmp = node_to_ir(child);
                if(tmp == nullptr)continue;

                if(child->node_type == RETURN_STMT){
                    return_node = tmp->codegen();
                }else{
                    tmp->codegen();
                }
            }
            llvm_builder->CreateRet(return_node);
            break;
    }
}

void generate_ir(std::shared_ptr<ast_node> &root) {
    llvm_context = std::make_unique<llvm::LLVMContext>();
    llvm_module =  std::make_unique<llvm::Module>     (JIT_MODULE_NAME, *llvm_context);
    llvm_builder = std::make_unique<llvm::IRBuilder<>>(llvm_module->getContext());

    node_to_ir(root);

#ifdef DEBUG_MODE
    llvm_module->print(llvm::errs(), nullptr, false, true);
#endif
}



#endif //LEXER_TEST_LLVM_IR_H
