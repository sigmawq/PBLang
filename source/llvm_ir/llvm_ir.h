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
#include "ForExprIR.h"

#include <fstream>

llvm::Function *doNothing;

llvm::Value *stmt_to_ir(std::shared_ptr<ast_node> &stmt);

std::unique_ptr<ExprIR> expr_to_ir(std::shared_ptr<ast_node> &expr_node);

std::unique_ptr<FuncIR> func_to_ir(std::shared_ptr<ast_node> &func_node);

void declarePrintf() {
    llvm::FunctionType *printfType =
            llvm::FunctionType::get(llvm::Type::getInt32Ty(*llvm_context), llvm::Type::getInt8PtrTy(*llvm_context),
                                    true);

    llvm::Function *printfFunc =
            llvm::Function::Create(printfType,
                                   llvm::Function::ExternalLinkage,
                                   "printf",
                                   llvm_module.get());
}

void declarePutchar(){
    llvm::FunctionType *putcharType =
            llvm::FunctionType::get(llvm::Type::getInt32Ty(*llvm_context), llvm::Type::getInt32Ty(*llvm_context),
                                    false);

    llvm::Function *putcharFunc =
            llvm::Function::Create(putcharType,
                                   llvm::Function::ExternalLinkage,
                                   "putchar",
                                   llvm_module.get());
}

void declareDoNothing() {
    llvm::FunctionType *doNothingType =
            llvm::FunctionType::get(llvm::Type::getVoidTy(*llvm_context), llvm::None,
                                    false);

    doNothing =
            llvm::Function::Create(doNothingType,
                                   llvm::Function::ExternalLinkage,
                                   "llvm.donothing",
                                   llvm_module.get());
}

void DoNothing(){
    llvm_builder->CreateCall(doNothing);
}


llvm::Value *if_to_ir(std::shared_ptr<ast_node> &if_node, std::shared_ptr<ast_node> &less_else){
    auto var = std::make_unique<IfExprIR>(if_node);
    var->if_cond = expr_to_ir(if_node->children[0])->codegen();
    var->if_false = nullptr;
    if(var->if_cond->getType()->isIntegerTy()){
        var->if_cond = llvm_builder->CreateICmpNE(var->if_cond,
                                                  llvm::ConstantInt::get(*llvm_context, llvm::APInt(var->if_cond->getType()->getIntegerBitWidth(), 0, true)),
                                                  "ifcond");
    }
    else{
        var->if_cond = llvm_builder->CreateFCmpONE(var->if_cond,
                                                   llvm::ConstantFP::get(*llvm_context, llvm::APFloat(0.0)),
                                                   "ifcond");
    }


    llvm::Function *F = llvm_builder->GetInsertBlock()->getParent();

    llvm::BasicBlock *ThenBB =
            llvm::BasicBlock::Create(*llvm_context, "then", F);
    llvm::BasicBlock *ElseBB = llvm::BasicBlock::Create(*llvm_context, "else");
    llvm::BasicBlock *MergeBB = llvm::BasicBlock::Create(*llvm_context, "ifcontrol");

    llvm_builder->CreateCondBr(var->if_cond, ThenBB, ElseBB);

    llvm_builder->SetInsertPoint(ThenBB);

    var->if_true = stmt_to_ir(if_node->children[1]);
    if (!var->if_true) {
        return nullptr;
    }
    llvm_builder->CreateBr(MergeBB);
    ThenBB = llvm_builder->GetInsertBlock();

    F->getBasicBlockList().push_back(ElseBB);
    llvm_builder->SetInsertPoint(ElseBB);

    if (if_node->children.size() == 3) {
        if_node->children[2]->node_type = IF;
        var->if_false = if_to_ir(if_node->children[2], less_else);   // recursively parse "else if"
    }
    else if (less_else != nullptr){
        var->if_false = stmt_to_ir(less_else);
        less_else = nullptr;
    }
    else{
        DoNothing();
    }


    llvm_builder->CreateBr(MergeBB);
    ElseBB = llvm_builder->GetInsertBlock();

    F->getBasicBlockList().push_back(MergeBB);
    llvm_builder->SetInsertPoint(MergeBB);
    auto PHIType = var->if_true->getType();
    auto reservedValues = (var->if_false?2:1);
    llvm::PHINode *PN =
            llvm_builder->CreatePHI(PHIType, reservedValues, "iftmp");

    PN->addIncoming(var->if_true, ThenBB);
    if(reservedValues == 2)PN->addIncoming(var->if_false, ElseBB);
    return PN;
}

llvm::Value *stmt_to_ir(std::shared_ptr<ast_node> &stmt) {
    if (stmt->node_type != STATEMENT_SEQUENCE)return nullptr;

    llvm::Value * return_val = nullptr;

    for (unsigned idx = 0; idx < stmt->children.size(); idx++) {
        auto child = stmt->children[idx];
        if (child->node_type == RETURN_STMT) {
            return expr_to_ir(child->children[0])->codegen();
        }
        else if (child->node_type == F_DECL) {
            auto func = std::make_unique<FuncIR>(child)->codegen();
            auto body = child->get_child(STATEMENT_SEQUENCE);

            llvm::BasicBlock *BB = llvm::BasicBlock::Create(*llvm_context, func->getName(), func);
            llvm_builder->SetInsertPoint(BB);

            named_values.clear();
            for (auto &Arg : func->args()) {
                llvm::AllocaInst *args_alloca = CreateEntryBlockAlloca(func, Arg.getName().str(), Arg.getType());
                llvm_builder->CreateStore(&Arg, args_alloca);
                named_values[std::string(Arg.getName())] = args_alloca;
            }
            auto return_value = stmt_to_ir(body);

            if(func->getReturnType()->isVoidTy()){
                llvm_builder->CreateRet(nullptr);
            }
            else {
                llvm_builder->CreateRet(return_value);
            }

            llvm::verifyFunction(*func);
        }
        else if (child->node_type == IF) {
            std::shared_ptr<ast_node> less_else = nullptr;
            if (idx != stmt->children.size() - 1 && stmt->children[idx + 1]->node_type == STATEMENT_SEQUENCE) {
                less_else = stmt->children[idx + 1];
                idx++;
            }

            return_val = if_to_ir(child, less_else);
        }
        else if (child->node_type == FOR) {
            auto for_obj = std::make_unique<ForExprIR>(child);
            for_obj->start = expr_to_ir(child->children[0]);
            for_obj->end = expr_to_ir(child->children[1]);
            for_obj->step = expr_to_ir(child->children[2]);

            auto start_val = for_obj->start->codegen();
            for_obj->var_name = start_val->getName().str();
            llvm::Function *F = llvm_builder->GetInsertBlock()->getParent();
            llvm::BasicBlock *pre_header_bb = llvm_builder->GetInsertBlock(),
                    *loop_bb = llvm::BasicBlock::Create(*llvm_context, "loop", F);
            llvm_builder->CreateBr(loop_bb);
            llvm_builder->SetInsertPoint(loop_bb);

            llvm::PHINode *variable = llvm_builder->CreatePHI(start_val->getType(), 2, for_obj->var_name);
            variable->addIncoming(start_val, pre_header_bb);


            auto oldVal = named_values[for_obj->var_name];
            //named_values[for_obj->var_name] = variable;
            auto body = child->get_child(STATEMENT_SEQUENCE);
            if (!body)
                return nullptr;

            stmt_to_ir(body);

            llvm::Value *step_val = nullptr;
            if (for_obj->step) {
                step_val = for_obj->step->codegen();
                if (!step_val)
                    return nullptr;
            }
            else {
                step_val = llvm::ConstantInt::get(*llvm_context, llvm::APInt(32, 1, true));
            }
            llvm::Value *next_var = nullptr;

            if(variable->getType()->isFloatTy()){
                if(!step_val->getType()->isFloatTy())
                    step_val = llvm_builder->CreateSIToFP(step_val, llvm::Type::getFloatTy(*llvm_context), "SItoFP");

                next_var = llvm_builder->CreateFAdd(variable, step_val, "nextvar");
            }
            else if(variable->getType()->isIntegerTy()){
                if(!step_val->getType()->isIntegerTy())
                    step_val = llvm_builder->CreateFPToSI(step_val, llvm::Type::getInt32Ty(*llvm_context), "FPtoSI");
                next_var = llvm_builder->CreateAdd(variable, step_val, "nextvar");
            }
            llvm::Value *end_cond = for_obj->end->codegen();

            if (!end_cond)
                return nullptr;

            if(variable->getType()->isFloatTy()){
                end_cond = llvm_builder->CreateFCmpONE(end_cond,
                                                       llvm::ConstantFP::get(*llvm_context,
                                                                             llvm::APFloat(0.0)),
                                                       "loopcond");
            }
            else{
                end_cond = llvm_builder->CreateICmpNE(end_cond,
                                                       llvm::ConstantInt::get(*llvm_context, llvm::APInt(end_cond->getType()->getIntegerBitWidth(), 0, true)),
                                                       "loopcond");
            }

            llvm::BasicBlock *loop_end_bb = llvm_builder->GetInsertBlock();
            llvm::BasicBlock *after_bb = llvm::BasicBlock::Create(*llvm_context, "afterloop", F);

            llvm_builder->CreateCondBr(end_cond, loop_bb, after_bb);
            llvm_builder->SetInsertPoint(after_bb);

            variable->addIncoming(next_var, loop_end_bb);

//            if (oldVal)
//                named_values[for_obj->var_name] = oldVal;
//            else
//                named_values.erase(for_obj->var_name);

        }
        else {
            return_val = expr_to_ir(child)->codegen();
        }

    }
    return return_val;
}

std::unique_ptr<ExprIR> expr_to_ir(std::shared_ptr<ast_node> &expr_node) {
    if (expr_node->is_operator()) {
        auto binary_call = std::make_unique<BinaryExprIR>(expr_node);
        binary_call->LHS = expr_to_ir(expr_node->children[0]);
        binary_call->RHS = expr_to_ir(expr_node->children[1]);
        return binary_call;
    } else if (expr_node->node_type == VAR_DECL) {
        auto var = std::make_unique<VariableExprIR>(expr_node);
        var->body = expr_to_ir(expr_node->children[2]);
        return var;
    } else if (expr_node->node_type == VAL_INT || expr_node->node_type == VAL_FP) {
        return std::make_unique<NumberExprIR>(expr_node);
    } else if (expr_node->node_type == VAL_STRING) {
        return std::make_unique<StringExprIR>(expr_node);
    } else if (expr_node->node_type == IDENTIFIER_AST) {
        auto identifier_call = std::make_unique<IdentifierExprIR>(expr_node);
        if (auto f_call = expr_node->get_child(F_CALL)) {
            identifier_call->isFuncCall = true;
            for (auto child: f_call->children) {
                identifier_call->args.push_back(expr_to_ir(child));
            }
        }
        return identifier_call;
    }
    return nullptr;
}

void generate_ir(std::shared_ptr<ast_node> &root) {
    LLVMInitializeNativeTarget();
    LLVMInitializeNativeAsmPrinter();
    LLVMInitializeNativeAsmParser();

    auto JTMB = llvm::orc::JITTargetMachineBuilder::detectHost();
    llvm_context = std::make_unique<llvm::LLVMContext>();
    llvm_module = std::make_unique<llvm::Module>(JIT_MODULE_NAME, *llvm_context);
    llvm_module->setDataLayout(*JTMB->getDefaultDataLayoutForTarget());
    llvm_module->setTargetTriple(JTMB->getTargetTriple().str());
    TheFPM = std::make_unique<llvm::legacy::FunctionPassManager>(llvm_module.get());

    llvm_builder = std::make_unique<llvm::IRBuilder<>>(llvm_module->getContext());

    declarePrintf();
    declarePutchar();
    declareDoNothing();

    stmt_to_ir(root);

#ifdef DEBUG_MODE
    llvm_module->print(llvm::errs(), nullptr, false, false);
#endif
    std::error_code EC;
    llvm::raw_fd_ostream Path("module_one", EC, llvm::sys::fs::F_None);
    llvm::WriteBitcodeToFile(*llvm_module, Path);

}


#endif //LEXER_TEST_LLVM_IR_H
