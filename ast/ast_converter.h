#ifndef LEXER_TEST_AST_CONVERTER_H
#define LEXER_TEST_AST_CONVERTER_H
#include "ast_node.h"

namespace std {
    template<typename T>
    using reff = std::reference_wrapper<T>;

    template<typename T>
    using opt = std::optional<T>;

    template<typename T>
    using uptr = std::unique_ptr<T>;

    template<typename T>
    using sptr = std::shared_ptr<T>;

    template<typename T>
    using wptr = std::weak_ptr<T>;
}


typedef std::shared_ptr<ast_node> ASTN;
static inline ASTN new_ASTN(AST_NODE_TYPE node_type) { return std::make_shared<ast_node>(node_type); }

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

static AST_NODE_TYPE convert_operator_to_ast_type_representation(std::string const& op) { return parse_node_operator_to_ast_node_type_mapping.at(op); }

static UNARY_OPERATOR convert_unary_operator_to_ast_type_representation(std::string const& op) { return parse_unary_node_operator_to_ast_node_type_mapping.at(op); }

extern std::optional<ASTN> convert_parse_tree_to_ast(parse_tree const& pt);

extern void handle_STMT(parse_tree const& pt, parse_node const& current_node_pn,
                                std::vector<std::shared_ptr<ast_node>> &action_sequence);

extern std::shared_ptr<ast_node> handle_BLOCK_SEGMENT(parse_tree const& pt, parse_node const& current_node_pn);

extern std::optional<std::shared_ptr<ast_node>> handle_STMT_topmost(parse_tree const& pt, parse_node const& current_node_pn);

extern std::shared_ptr<ast_node> handle_IF_STMT(parse_tree const& pt, parse_node const& current_node_pn);

extern ASTN handle_VAR_DECL(parse_tree const& pt, parse_node const& current_node_pn);

extern ASTN get_variable_signature(parse_tree const& pt, parse_node const& current_node_pn);

extern std::optional<ASTN> handle_opt_complex_assignment(parse_tree const& pt, parse_node const& cn);

extern ASTN handle_complex_assignment(parse_tree const& pt, parse_node const& cn);

extern ASTN handle_CONST_DECL(parse_tree const& pt, parse_node const& cn);

extern std::optional<ASTN> handle_OPT_VAR_NEXT(parse_tree const& pt, parse_node const& current_node_pn, ASTN &var_type);

extern ASTN handle_CONST_DECL(parse_tree const& pt, parse_node const& current_node_pn);

extern ASTN handle_ARR_DECL(parse_tree const& pt, parse_node const& current_node_pn);

extern ASTN handle_F_DECL(parse_tree const& pt, parse_node const& current_node_pn);

extern ASTN handle_F_DECL_OVERLOAD(parse_tree const& pt, parse_node const& current_node_pn);

extern ASTN handle_STRUCT_DECL(parse_tree const& pt, parse_node const& current_node_pn);

extern ASTN handle_WHILE_STMT(parse_tree const& pt, parse_node const& current_node_pn);

extern ASTN handle_FOR_STMT(parse_tree const& pt, parse_node const& current_node_pn);

extern std::vector<std::shared_ptr<ast_node>> handle_ELIF(parse_tree const& pt, parse_node const& current_node_pn);

extern void handle_ELIF_recursive(parse_tree const& pt, parse_node const& current_node_pn,
                           std::vector<std::shared_ptr<ast_node>> &elifs_gathered);

extern std::shared_ptr<ast_node> handle_AEs(parse_tree const& pt, parse_node const& current_node_pn);

extern void handle_AEs_nonresolving(parse_tree const& pt, parse_node const& current_node_pn,
                                                         std::vector<std::shared_ptr<ast_node>> &arithmetic_expression_parsed);

extern void handle_AE(parse_tree const& pt, parse_node const& current_node_pn, std::vector<std::shared_ptr<ast_node>> &arithmetic_expression_parsed);

extern void handle_PARANT_AE(parse_tree const& pt, parse_node const& current_node_pn, std::vector<std::shared_ptr<ast_node>> &arithmetic_expression_parsed);

extern std::shared_ptr<ast_node>  handle_F(parse_tree const& pt, parse_node const& current_node_pn);

extern std::optional<UNARY_OPERATOR> handle_OPT_UNARY_OP(parse_tree const& pt, parse_node const& current_node_pn);

extern void handle_OPT_F_TRAIL(parse_tree const& pt, parse_node const& current_node_pn,
                                                                 std::vector<std::shared_ptr<ast_node>> &operations_on_object_array);

extern void handle_F_TRAIL(parse_tree const& pt, parse_node const& current_node_pn,
                                                             std::vector<std::shared_ptr<ast_node>> &operations_on_object_array);

extern std::shared_ptr<ast_node> handle_F_CALL(parse_tree const& pt, parse_node const& current_node_pn);

extern ASTN handle_ARR_SIGN(parse_tree const& pt, parse_node const& cn);

extern void handle_NEXT_DIM_DECL(parse_tree const& pt, parse_node const& cn,
                          std::vector<ASTN> &dim_group);

extern ASTN handle_DIM_DECL(parse_tree const& pt, parse_node const& cn);

extern std::optional<ASTN> handle_OPT_DIM_DECL(parse_tree const& pt, parse_node const& cn);

extern ASTN get_identifier(parse_tree const& pt, parse_node const& cn);

extern ASTN handle_F_DECL(parse_tree const& pt, parse_node const& cn);

extern ASTN handle_ARG_DECL(parse_tree const& pt, parse_node const& cn);

extern ASTN handle_OPT_ARG_DECL(parse_tree const& pt, parse_node const& cn,
                                std::vector<ASTN> &all_args);

extern ASTN handle_F_INPUT(parse_tree const& pt, parse_node const& cn);

extern ASTN handle_F_OUTPUT(parse_tree const& pt, parse_node const& cn);

extern ASTN handle_F_DECL_OVERLOAD(parse_tree const& pt, parse_node & cn);

extern ASTN handle_STRUCT_DECL(parse_tree const& pt, parse_node const& cn);

extern void handle_OPT_STRUCT_VAR_DECL(parse_tree const& pt, parse_node const& cn, std::opt<std::vector<ASTN>> &var_container);

extern ASTN handle_RETURN_STMT(parse_tree const& pt, parse_node const& cn);

extern ASTN handle_WHILE_STMT(parse_tree const& pt, parse_node const& cn);

extern std::vector<ASTN> handle_ARG_CALL_S(parse_tree const& pt, parse_node const& cn);

extern void handle_ARG_CALL(parse_tree const& pt, parse_node const& cn, std::vector<ASTN> &all_args);

#endif //LEXER_TEST_AST_CONVERTER_H
