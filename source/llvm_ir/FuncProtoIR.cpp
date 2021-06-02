//
// Created by foxfurry on 6/1/21.
//

#include "FuncProtoIR.h"

llvm::Function *FuncProtoIR::codegen(std::shared_ptr<ast_node> node) {
    if (auto func_just_text = node->get_child(AST_JUST_TEXT)) {
        func_name = func_just_text->optional_value->value;
    }
    else { return nullptr; }

    llvm::Type *return_type = llvm::Type::getVoidTy(*llvm_context);
    std::vector<llvm::Type *> args_type;

    if(node->get_child(F_INPUT_VOID)){ args_type.clear(); }
    else if(auto f_input = node->get_child(F_INPUT))
        for(auto f_arg: f_input->children){
            auto type_spec = f_arg->get_child(VARIABLE_SIGNATURE)->get_child(TYPE_SPEC)->optional_value->value;
            auto arg_name = f_arg->get_child(AST_JUST_TEXT)->optional_value->value;

            args_type.push_back(str_to_type(type_spec));
            args_name.push_back(arg_name);
        }
    else{ return nullptr; }

    if(auto f_output = node->get_child(F_OUTPUT)){
        auto type_spec = f_output->get_child(VARIABLE_SIGNATURE)->get_child(TYPE_SPEC)->optional_value->value;
        return_type = str_to_type(type_spec);
    }

    llvm::FunctionType *FT =
            llvm::FunctionType::get(return_type, args_type, false);

    llvm::Function *F =
            llvm::Function::Create(FT,
                                   llvm::Function::ExternalLinkage,
                                   func_name,
                                   llvm_module.get());

    // Set names for all arguments.
    unsigned Idx = 0;
    for (auto &Arg : F->args())
        Arg.setName(args_name[Idx++]);

    return F;
}

std::unique_ptr<FuncProtoIR> LogErrorP(const char *Str) {
    LogError(Str);
    return nullptr;
}