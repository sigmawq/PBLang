//
// Created by foxfurry on 5/24/21.
//

#ifndef LEXER_TEST_LLVM_IR_H
#define LEXER_TEST_LLVM_IR_H

#include <llvm/ADT/APFloat.h>
#include <llvm/ADT/STLExtras.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Verifier.h>

#include "ast/ast_node.h"

#define JIT_MODULE_NAME "PBLANG JIT MODULE"

static std::unique_ptr<llvm::LLVMContext> llvm_context;
static std::unique_ptr<llvm::Module> llvm_module;
static std::unique_ptr<llvm::IRBuilder<>> llvm_builder;
static std::map<std::string, llvm::Value *> named_values;

void initialize_ir(){
    llvm_context  = std::make_unique<llvm::LLVMContext>();
    llvm_module   = std::make_unique<llvm::Module>(JIT_MODULE_NAME, *llvm_context);
    llvm_builder  = std::make_unique<llvm::IRBuilder<>>(*llvm_context);
}


llvm::Type* ir_str_to_type(std::string str_type){
    if(str_type == "int")   return llvm::Type::getInt64Ty(*llvm_context);
    if(str_type == "float") return llvm::Type::getFloatTy(*llvm_context);
    if(str_type == "double")return llvm::Type::getDoubleTy(*llvm_context);
    if(str_type == "bool")  return llvm::Type::getInt1Ty(*llvm_context);
    if(str_type == "char")  return llvm::Type::getInt1Ty(*llvm_context);

    return nullptr;
}


std::shared_ptr<ast_node> get_child(std::shared_ptr<ast_node> &node, AST_NODE_TYPE search_type){
    for(auto child: node->children) if (child->node_type == search_type) return child; return nullptr;
}

std::vector<llvm::Type*> get_func_args_types(std::shared_ptr<ast_node> &func_node){
    std::vector<llvm::Type*> result;
    int arguments = 0;
    if (get_child(func_node, F_INPUT_VOID) == nullptr){
        for(auto f_input: get_child(func_node, F_INPUT)->children){
            arguments++;
            if(auto signature = get_child(f_input, VARIABLE_SIGNATURE)){
                llvm::Type* signature_type = ir_str_to_type(get_child(signature,TYPE_SPEC)->optional_value->value);
                if(signature_type != nullptr){
                    result.push_back(signature_type);
                }
            }
        }
    }
    else{
        result.push_back(llvm::Type::getVoidTy(*llvm_context));
    }
    return result;
}

llvm::Function * func_to_ir(std::shared_ptr<ast_node> &func_node){
    llvm::Type *return_type;
    if(get_child(func_node, F_OUTPUT_VOID) != nullptr){
        return_type = llvm::Type::getVoidTy(*llvm_context);
    }
    else{
        auto f_output = get_child(func_node, F_OUTPUT);
        if(auto signature = get_child(f_output,VARIABLE_SIGNATURE)){
            llvm::Type* signature_type = ir_str_to_type(get_child(signature,TYPE_SPEC)->optional_value->value);
            if(signature_type != nullptr){
                return_type = signature_type;
            }
        }
    }

    std::string func_name = get_child(func_node, AST_JUST_TEXT)->optional_value->value;
    auto arguments_type = get_func_args_types(func_node);

#ifdef DEBUG_MODE
    std::cout << "[DEBUG] Generating IR for " << func_name << " with " << arguments_type.size() << " arguments_type\n";
#endif

    llvm::FunctionType *FT = llvm::FunctionType::get(return_type, arguments_type, false);
    llvm::Function *F = llvm::Function::Create(FT, llvm::Function::ExternalLinkage, func_name, llvm_module.get());
    return F;
}

void node_to_ir(std::shared_ptr<ast_node> &node){
    switch (node->node_type){
        case F_DECL:
            func_to_ir(node);
            break;     // Function declaration

        case VAR_DECL: break;   // Global variables
        case CONST_DECL: break; // Global variables
        case ARR_DECL: break;   // Global variables
        case STRUCT_DECL: break;// Global variables

    }
}


void generate_ir(std::shared_ptr<ast_node> &root){
    initialize_ir();
    for(auto child: root->children){
        node_to_ir(child);
    }

#ifdef DEBUG_MODE
    llvm_module->print(llvm::errs(), nullptr);
#endif
}

#endif //LEXER_TEST_LLVM_IR_H
