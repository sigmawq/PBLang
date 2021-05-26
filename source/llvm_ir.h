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

void initialize_ir() {
    llvm_context = std::make_unique<llvm::LLVMContext>();
    llvm_module = std::make_unique<llvm::Module>(JIT_MODULE_NAME, *llvm_context);
    llvm_builder = std::make_unique<llvm::IRBuilder<>>(*llvm_context);
}


llvm::Type *ir_str_to_type(std::string str_type) {
    if (str_type == "int") return llvm::Type::getInt64Ty(*llvm_context);
    if (str_type == "float") return llvm::Type::getFloatTy(*llvm_context);
    if (str_type == "double")return llvm::Type::getDoubleTy(*llvm_context);
    if (str_type == "bool") return llvm::Type::getInt1Ty(*llvm_context);
    if (str_type == "char") return llvm::Type::getInt1Ty(*llvm_context);

    return nullptr;
}


std::shared_ptr<ast_node> get_child(std::shared_ptr<ast_node> &node, AST_NODE_TYPE search_type) {
    for (auto child: node->children) if (child->node_type == search_type) return child;
    return nullptr;
}

std::pair<std::vector<llvm::Type *>, std::vector<std::string>> get_func_args_types(std::shared_ptr<ast_node> &func_node) {
    std::pair<std::vector<llvm::Type *>, std::vector<std::string>> result;
    int arguments = 0;
    if (get_child(func_node, F_INPUT_VOID) == nullptr) {
        for (auto f_arg: get_child(func_node, F_INPUT)->children) {
            arguments++;
            if (auto signature = get_child(f_arg, VARIABLE_SIGNATURE)) {
                llvm::Type *signature_type = ir_str_to_type(get_child(signature, TYPE_SPEC)->optional_value->value);
                std::string func_name = get_child(f_arg, AST_JUST_TEXT)->optional_value->value;
                if (signature_type != nullptr) {
                    result.first.push_back(signature_type);
                    result.second.push_back(func_name);
                }
            }
        }
    } else {
        result.first.push_back(llvm::Type::getVoidTy(*llvm_context));
        result.second.push_back("");
    }
    return result;
}

llvm::Function *func_to_ir(std::shared_ptr<ast_node> &func_node) {
    llvm::Type *return_type = llvm::Type::getVoidTy(*llvm_context);;
    if (get_child(func_node, F_OUTPUT_VOID) == nullptr) {
        auto f_output = get_child(func_node, F_OUTPUT);
        if (auto signature = get_child(f_output, VARIABLE_SIGNATURE)) {
            llvm::Type *signature_type = ir_str_to_type(get_child(signature, TYPE_SPEC)->optional_value->value);
            if (signature_type != nullptr) {
                return_type = signature_type;
            }
        }
    }

    std::string func_name = get_child(func_node, AST_JUST_TEXT)->optional_value->value;
    auto arguments_type = get_func_args_types(func_node);

#ifdef DEBUG_MODE
    std::cout << "[DEBUG] Generating IR for " << func_name << " with " << arguments_type.first.size() << " arguments\n";
#endif

    llvm::FunctionType *FT = llvm::FunctionType::get(return_type, arguments_type.first, false);
    llvm::Function *F = llvm::Function::Create(FT, llvm::Function::ExternalLinkage, func_name, llvm_module.get());

    unsigned idx = 0;
    for(auto &Arg: F->args()){
       Arg.setName(arguments_type.second[idx++]);
    }

    return F;
}

llvm::Value *var_to_ir(std::shared_ptr<ast_node> &var_node) {
    std::string var_name = get_child(var_node, AST_JUST_TEXT)->optional_value->value;
    llvm::Value *V = named_values[var_name];
    if (!V) {
        std::cout << "If you see this - something is terribly wrong\n";
    }
    return V;

}

llvm::Value *num_to_ir(std::shared_ptr<ast_node> &var_node) {
    float val = 0;
    switch (var_node->node_type) {
        case VAL_INT:
            val = (int) std::stoi(var_node->optional_value->value);
            return llvm::ConstantFP::get(*llvm_context, llvm::APFloat(val));
        case VAL_FP:
            val = std::stof(var_node->optional_value->value);
            return llvm::ConstantFP::get(*llvm_context, llvm::APFloat(val));
    }
}

llvm::Value *expr_to_ir(std::shared_ptr<ast_node> &expr_node) {
    if (expr_node->children.size() == 2) {
        llvm::Value *LHS = expr_to_ir(expr_node->children[0]);
        llvm::Value *RHS = expr_to_ir(expr_node->children[1]);

        switch (expr_node->node_type) {
            case ADD:
                return llvm_builder->CreateFAdd(LHS, RHS, "addtmp");
            case SUB:
                return llvm_builder->CreateFSub(LHS, RHS, "subtmp");
            case MUL:
                return llvm_builder->CreateFMul(LHS, RHS, "multmp");
            case DIV:
                return llvm_builder->CreateFDiv(LHS, RHS, "divtmp");
            case COMP_GREATER:
                return llvm_builder->CreateUIToFP(LHS, llvm::Type::getDoubleTy(*llvm_context), "addtmp");
        }
    }
    else return num_to_ir(expr_node);
}

void node_to_ir(std::shared_ptr<ast_node> &node) {
    switch (node->node_type) {
        case F_DECL:
            func_to_ir(node);
            break;     // Function declaration

        case VAR_DECL:
            var_to_ir(node);
            break;   // Global variables
        case CONST_DECL:
            break; // Global variables
        case ARR_DECL:
            break;   // Global variables
        case STRUCT_DECL:
            break;// Global variables

        case ADD:
        case SUB:
        case DIV:
        case MUL:
        case COMP_GREATER:
            expr_to_ir(node);


    }
}


void generate_ir(std::shared_ptr<ast_node> &root) {
    initialize_ir();
    for (auto child: root->children) {
        node_to_ir(child);
    }

#ifdef DEBUG_MODE
    llvm_module->print(llvm::errs(), nullptr, false, true);
#endif
}

#endif //LEXER_TEST_LLVM_IR_H
