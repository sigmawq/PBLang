#ifndef LEXER_TEST_AST_CONVERTER_H
#define LEXER_TEST_AST_CONVERTER_H
#include "ast_node.h"

static const std::unordered_map<std::string, AST_NODE_TYPE> parse_node_operator_to_ast_node_type_mapping{
        {"+", ADD},
        {"-", SUB},
        {"*", MUL},
        {"/", DIV},
        {"||", LOGICAL_OR},
        {"&&", LOGICAL_AND},
        {"==", COMP_EQUALS},
        {">", COMP_GREATER},
        {">=", COMP_GREATER_EQUALS},
        {"<", COMP_LESS},
        {"<=", COMP_LESS_EQUALS},
};

static const std::unordered_map<std::string, UNARY_OPERATOR> parse_unary_node_operator_to_ast_node_type_mapping{
        {"!", LOGICAL_NOT},
        {"-", MINUS},
        {"+", PLUS},
        {"|cast|", PLUS},
};

extern AST_NODE_TYPE convert_operator_to_ast_type_representation(std::string const& op) { return parse_node_operator_to_ast_node_type_mapping.at(op); }

extern UNARY_OPERATOR convert_unary_operator_to_ast_type_representation(std::string const& op) { return parse_unary_node_operator_to_ast_node_type_mapping.at(op); }

extern std::unique_ptr<ast_node> convert_parse_tree_to_ast(parse_tree const& pt);

extern std::vector<std::shared_ptr<ast_node>> handle_IF_STMT(parse_tree const& pt, parse_node const& current_node_pn);

extern std::vector<std::shared_ptr<ast_node>> handle_AEs(parse_tree const& pt, parse_node const& current_node_pn);

extern std::vector<std::shared_ptr<ast_node>> handle_AE(parse_tree const& pt, parse_node const& current_node_pn);

extern std::vector<std::shared_ptr<ast_node>> handle_F(parse_tree const& pt, parse_node const& current_node_pn);

extern std::optional<UNARY_OPERATOR> handle_OPT_UNARY_OP(parse_tree const& pt, parse_node const& current_node_pn);

#endif //LEXER_TEST_AST_CONVERTER_H
