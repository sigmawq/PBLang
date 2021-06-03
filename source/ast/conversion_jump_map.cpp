//
// Created by swqd on 4/6/21.
//

#include "conversion_jump_map.h"
#include "ast_converter.h"

/*std::unordered_map<std::string, std::unordered_map<std::string, conversion_function_t>> conversion_jump_map::container{
        {"STMT", {
                         {"STMT", ast_converter::generic_ast_jump},
                         {"A_Es", ast_converter::generic_ast_jump},
                         {";", ast_converter::ast_jump_ignore},
                         {"VAR_DECL", ast_converter::generic_ast_jump},
                         {"IF_STMT", ast_converter::handle_IF_STMT},
                 }},

        {
            "IF_STMT", {
                               {"if", ast_converter::ast_jump_ignore},
                               {"(", ast_converter::ast_jump_ignore},
                               {")", ast_converter::ast_jump_ignore},
                               {"A_Es", ast_converter::handle_A_Es},
                               {"BLOCK_SEGMENT", ast_converter::handle_BLOCK_SEGMENT},
                               {"OPT_ELSE", ast_converter::handle_BLOCK_SEGMENT},
            }
        }
};

std::optional<conversion_function_t> conversion_jump_map::get_jump_function(const std::string &jumpable_gu, const std::string &jump_at) {
    try{
        return container.at(jumpable_gu).at(jump_at);
    }
    catch (...){
        return {};
    }
}*/