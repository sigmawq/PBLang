//
// Created by swqd on 4/4/21.
//

#include "ast_converter.h"
#include "AE_parser.h"

std::unique_ptr<ast_node> convert_parse_tree_to_ast(parse_tree const& pt){
    auto program_root = std::make_unique<ast_node>(AST_NODE_TYPE::STATEMENT_SEQUENCE);

    if (pt.get_root().gu.string_representation == "STMT"){
        auto astn = handle_STMT_topmost(pt, pt.get_root());
        if (astn.has_value()){
            program_root->add_child(astn.value());
        }
    }
    else{
        throw std::runtime_error("Parse node root is not STMT, but should always be!");
    }

    return program_root;
}

void handle_STMT(parse_tree const& pt, parse_node const& current_node_pn,
                                                     std::vector<std::shared_ptr<ast_node>> &action_sequence){
    if (current_node_pn.children.empty()) return;
    auto &first_pn = pt.get_node_const(current_node_pn.children.back());
    if (first_pn.gu.string_representation == "A_Es"){
        action_sequence.push_back(handle_AEs(pt, first_pn));
    }
    else if (first_pn.gu.string_representation == "STMT"){
        handle_STMT(pt, first_pn, action_sequence);
    }
    else{
        throw std::runtime_error("WIP");
    }
}

std::optional<std::shared_ptr<ast_node>> handle_STMT_topmost(parse_tree const& pt, parse_node const& current_node_pn){
    std::shared_ptr<ast_node> action_sequence_astn = std::make_shared<ast_node>(STATEMENT_SEQUENCE);
    std::vector<std::shared_ptr<ast_node>> action_sequence;

    handle_STMT(pt, current_node_pn, action_sequence);
    handle_STMT(pt, pt.get_node_const(current_node_pn.children[0]), action_sequence);

    if (!action_sequence.empty()){
        for (auto& node : action_sequence){
            action_sequence_astn->add_child(node);
        }
        return action_sequence_astn;
    }
    else{
        return {};
    }
}

std::shared_ptr<ast_node> handle_IF_STMT(parse_tree const& pt, parse_node const& current_node_pn){
    auto const& condition_pn = pt.get_node_const(current_node_pn.children[2]);
    auto const& exec_if_true_pn = pt.get_node_const(current_node_pn.children[4]);
    auto const& exec_if_false_pn = pt.get_node_const(current_node_pn.children[5]);

    auto condition_astn = handle_AEs(pt, condition_pn);
    auto exec_if_true_astn = handle_AEs(pt, exec_if_true_pn);
    auto exec_if_false_astn = handle_AEs(pt, exec_if_false_pn);

    throw std::runtime_error("WIP");
}

std::shared_ptr<ast_node> handle_AEs(parse_tree const& pt, parse_node const& current_node_pn){
    auto const& factor_pn = pt.get_node_const(current_node_pn.children.back());
    auto const& AE_pn = pt.get_node_const(current_node_pn.children[0]);

    std::vector<std::shared_ptr<ast_node>> arithmetic_expression_parsed;
    arithmetic_expression_parsed.push_back(handle_F(pt, factor_pn));

    handle_AE(pt, AE_pn, arithmetic_expression_parsed);

    AE_parser parser { arithmetic_expression_parsed };
    return parser.convert_to_ast();
}

void handle_AE(parse_tree const& pt, parse_node const& current_node_pn,
                                                 std::vector<std::shared_ptr<ast_node>> &arithmetic_expression_parsed){
    if (current_node_pn.children.empty()) return;

    auto const& sign_pn = pt.get_node_const(current_node_pn.children.back());
    auto const& factor_pn = pt.get_node_const(current_node_pn.children[1]);
    auto const& recursive_next_AE_pn = pt.get_node_const(current_node_pn.children[0]);

    std::shared_ptr<ast_node> next_operator = std::make_shared<ast_node>(
            convert_operator_to_ast_type_representation(sign_pn.gu.string_representation)
    );
    arithmetic_expression_parsed.push_back(next_operator);

    arithmetic_expression_parsed.push_back(handle_F(pt, factor_pn));

    handle_AE(pt, recursive_next_AE_pn, arithmetic_expression_parsed);
}

std::shared_ptr<ast_node> handle_F(parse_tree const& pt, parse_node const& current_node_pn){
    auto const& first_symbol_pn = pt.get_node_const(current_node_pn.children.back());

    std::shared_ptr<ast_node> factor = std::make_shared<ast_node>(AST_NODE_TYPE::AST_INVALID);

    factor->optional_value.emplace();
    if (first_symbol_pn.gu.string_representation == "OPT_UNARY_OP"){
        std::optional<UNARY_OPERATOR> unary_operator = handle_OPT_UNARY_OP(pt, first_symbol_pn);
        factor->node_type = AST_NODE_TYPE::IDENTIFIER_AST;
        auto const& identifier_pn = pt.get_node_const(current_node_pn.children[1]);
        factor->optional_value->value = identifier_pn.optional_token->attribute; // Unsure about this CHECK
        factor->optional_value->optional_unary_operator = unary_operator;
        std::vector<std::shared_ptr<ast_node>> trailing_operators;
        handle_OPT_F_TRAIL(pt, pt.get_node_const(current_node_pn.children[0]), trailing_operators);
        for (auto& trailing_op : trailing_operators){
            factor->add_child(trailing_op);
        }
    }
    else if (first_symbol_pn.gu.is_number){
        if (first_symbol_pn.optional_token->type == TOKEN_TYPE::CONSTANT_INTEGER){
            factor->node_type = VAL_INT;
        }
        else if (first_symbol_pn.optional_token->type == TOKEN_TYPE::CONSTANT_FP){
            factor->node_type = VAL_FP;
        }
        else{
            throw std::exception{};
        }
        factor->optional_value.value().value = first_symbol_pn.optional_token->attribute;
    }
    else if (first_symbol_pn.gu.is_string){
        factor->node_type = VAL_STRING;
        factor->optional_value.value().value = first_symbol_pn.optional_token->attribute;
    }
    else {
        throw std::runtime_error("Failed to parse F");
    }
    return factor;
}

extern std::optional<UNARY_OPERATOR> handle_OPT_UNARY_OP(parse_tree const& pt, parse_node const& current_node_pn){
    if (current_node_pn.children.empty()) return {};
    else{
        std::string unary_op_string = pt.get_node_const(current_node_pn.children.back()).gu.string_representation;
        return {convert_unary_operator_to_ast_type_representation(unary_op_string)};
    }
}

void handle_OPT_F_TRAIL(parse_tree const& pt, parse_node const& current_node_pn,
                                             std::vector<std::shared_ptr<ast_node>> &operations_on_object_array){
    if (current_node_pn.children.empty()) return;
    else{
        handle_F_TRAIL(pt, pt.get_node_const(current_node_pn.children[1]), operations_on_object_array);
        handle_OPT_F_TRAIL(pt, pt.get_node_const(current_node_pn.children[0]), operations_on_object_array);
    }
}

void handle_F_TRAIL(parse_tree const& pt, parse_node const& current_node_pn,
                                         std::vector<std::shared_ptr<ast_node>> &operations_on_object_array){
    auto const& only_node = pt.get_node_const(current_node_pn.children[0]);

    if (only_node.gu.string_representation == "F_CALL"){
        const auto& f_call_astn = handle_F_CALL(pt, current_node_pn);
        operations_on_object_array.push_back(f_call_astn);
    }
    else if (only_node.gu.string_representation == "OPT_ARR"){
        throw std::runtime_error("WIP");
    }
    else if (only_node.gu.string_representation == "STRUCT_ACCESS"){
        throw std::runtime_error("WIP");
    }
    else{
        throw std::exception{};
    }
}

std::shared_ptr<ast_node> handle_F_CALL(parse_tree const& pt, parse_node const& current_node_pn){
    throw std::runtime_error("WIP");
}