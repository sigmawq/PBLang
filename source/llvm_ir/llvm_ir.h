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
#include "VariableExprIR.h"
#include "IfExprIR.h"
#include "StringExprIR.h"

llvm::Value* stmt_to_ir(std::shared_ptr<ast_node> &stmt);
std::unique_ptr<ExprIR> expr_to_ir(std::shared_ptr<ast_node> &expr_node);
std::unique_ptr<FuncIR> func_to_ir(std::shared_ptr<ast_node> &func_node);

llvm::Value* stmt_to_ir(std::shared_ptr<ast_node> &stmt){
    if(stmt->node_type != STATEMENT_SEQUENCE)return nullptr;

    for(unsigned idx = 0; idx < stmt->children.size(); idx++){
        auto child = stmt->children[idx];
        if(child->node_type == RETURN_STMT){
            return expr_to_ir(child->children[0])->codegen();
        }
        else if(child->node_type == F_DECL){
            auto func = std::make_unique<FuncIR>(child)->codegen();
            auto body = child->get_child(STATEMENT_SEQUENCE);

            llvm::BasicBlock *BB = llvm::BasicBlock::Create(*llvm_context, func->getName(), func);
            llvm_builder->SetInsertPoint(BB);

            auto return_value = stmt_to_ir(body);
            llvm_builder->SetInsertPoint(BB);
            llvm_builder->CreateRet(return_value);

            llvm::verifyFunction(*func);
        }
        else if(child->node_type == VAR_DECL){
            auto var = std::make_unique<VariableExprIR>(child);
            var->body = expr_to_ir(child->children[2]);
            var->codegen();
        }
        else if(child->node_type == IF){
            auto var = std::make_unique<IfExprIR>(child);
            var->if_cond = expr_to_ir(child->children[0])->codegen();
            var->if_cond = llvm_builder->CreateFCmpONE(var->if_cond, llvm::ConstantFP::get(*llvm_context, llvm::APFloat(0.0)), "ifcond");

            llvm::Function *F = llvm_builder->GetInsertBlock()->getParent();

            llvm::BasicBlock *ThenBB =
                    llvm::BasicBlock::Create(*llvm_context, "then", F);
            llvm::BasicBlock *ElseBB = llvm::BasicBlock::Create(*llvm_context, "else");
            llvm::BasicBlock *MergeBB = llvm::BasicBlock::Create(*llvm_context, "ifcond");

            llvm_builder->CreateCondBr(var->if_cond, ThenBB, ElseBB);

            llvm_builder->SetInsertPoint(ThenBB);
            var->if_true = stmt_to_ir(child->children[1]);
            if(!var->if_true){
                return nullptr;
            }
            llvm_builder->CreateBr(MergeBB);
            ThenBB = llvm_builder->GetInsertBlock();

            F->getBasicBlockList().push_back(ElseBB);
            llvm_builder->SetInsertPoint(ElseBB);

            if(child->children.size()>2){
                var->if_false = stmt_to_ir(child->children[2]);
            }
            else if(idx != stmt->children.size()-1 && stmt->children[idx+1]->node_type == STATEMENT_SEQUENCE){
                var->if_false = stmt_to_ir(stmt->children[idx+1]);
                idx++;
            }
            if(!var->if_false){
                return nullptr;
            }
            llvm_builder->CreateBr(MergeBB);
            ElseBB = llvm_builder->GetInsertBlock();

            F->getBasicBlockList().push_back(MergeBB);
            llvm_builder->SetInsertPoint(MergeBB);
            auto PHIType = var->if_true->getType();
            llvm::PHINode *PN =
                    llvm_builder->CreatePHI(PHIType, 2, "iftmp");

            PN->addIncoming(var->if_true, ThenBB);
            PN->addIncoming(var->if_false, ElseBB);
            return PN;
        }
        else {
            expr_to_ir(child)->codegen();
        }

    }
    return nullptr;
}

std::unique_ptr<ExprIR> expr_to_ir(std::shared_ptr<ast_node> &expr_node){
    if(expr_node->is_operator()){
        auto binary_call = std::make_unique<BinaryExprIR>(expr_node);
        binary_call->LHS = expr_to_ir(expr_node->children[0]);
        binary_call->RHS = expr_to_ir(expr_node->children[1]);
        return binary_call;
    }
    else if(expr_node->node_type == VAL_INT || expr_node->node_type == VAL_FP){
        return std::make_unique<NumberExprIR>(expr_node);
    }
    else if(expr_node->node_type == VAL_STRING){
        return std::make_unique<StringExprIR>(expr_node);
    }
    else if(expr_node->node_type == IDENTIFIER_AST){
        auto identifier_call = std::make_unique<IdentifierExprIR>(expr_node);
        if(auto f_call = expr_node->get_child(F_CALL)){
            identifier_call->isFuncCall = true;
            for(auto child: f_call->children){
                identifier_call->args.push_back(expr_to_ir(child));
            }
        }
        return identifier_call;
    }
    return nullptr;
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
