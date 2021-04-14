//
// Created by swqd on 4/4/21.
//

#include "ast_converter.h"
#include "AE_parser.h"

std::unique_ptr<ast_node> convert_parse_tree_to_ast(parse_tree const& pt){
    auto program_root = std::make_unique<ast_node>(AST_NODE_TYPE::PROGRAM_ROOT);

}

std::vector<std::shared_ptr<ast_node>> handle_IF_STMT(parse_tree const& pt, parse_node const& current_node_pn){
    auto const& condition_pn = pt.get_node_const(current_node_pn.children[2]);
    auto const& exec_if_true_pn = pt.get_node_const(current_node_pn.children[4]);
    auto const& exec_if_false_pn = pt.get_node_const(current_node_pn.children[5]);

    auto condition_astn = handle_AEs(pt, condition_pn);
    auto exec_if_true_astn = handle_AEs(pt, condition_pn);
    auto exec_if_false_astn = handle_AEs(pt, condition_pn);
}

std::vector<std::shared_ptr<ast_node>> handle_AEs(parse_tree const& pt, parse_node const& current_node_pn){
    auto const& factor_pn = pt.get_node_const(current_node_pn.children[0]);
    auto const& AE_pn = pt.get_node_const(current_node_pn.children[1]);

    std::vector<std::shared_ptr<ast_node>> arithmetic_expression_parsed;
}

std::vector<std::shared_ptr<ast_node>> handle_AE(parse_tree const& pt, parse_node const& current_node_pn,
                                                 std::vector<std::shared_ptr<ast_node>> &arithmetic_expression_parsed){
    if (current_node_pn.children.empty()) return {};

    auto const& sign_pn = pt.get_node_const(current_node_pn.children[0]);
    auto const& factor_pn = pt.get_node_const(current_node_pn.children[1]);
    auto const& recursive_next_AE_pn = pt.get_node_const(current_node_pn.children[2]);
}

std::vector<std::shared_ptr<ast_node>> handle_F(parse_tree const& pt, parse_node const& current_node_pn){
    // Decide what kind of factor it is
    auto const& first_symbol_pn = pt.get_node_const(current_node_pn.children[0]);

    if (first_symbol_pn.gu.string_representation == "OPT_UNARY_OP"){
        std::optional<UNARY_OPERATOR> unary_operator = handle_OPT_UNARY_OP(pt, first_symbol_pn);
    }
    else if (first_symbol_pn.gu.is_number){

    }
    else if (first_symbol_pn.gu.is_string){

    }
    else {
        throw std::runtime_error("Failed to parse F");
    }

}

extern std::optional<UNARY_OPERATOR> handle_OPT_UNARY_OP(parse_tree const& pt, parse_node const& current_node_pn){
    if (current_node_pn.children.empty()) return {};
    else{
        std::string unary_op_string = pt.get_node_const(current_node_pn.children[0]).gu.string_representation;
        return {convert_unary_operator_to_ast_type_representation(unary_op_string)};
    }
}
