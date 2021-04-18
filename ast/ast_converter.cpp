//
// Created by swqd on 4/4/21.
//

#include "ast_converter.h"
#include "AE_parser.h"


std::optional<ASTN> convert_parse_tree_to_ast(parse_tree const& pt){
    if (pt.get_root().gu.string_representation == "STMT"){
        auto astn = handle_STMT_topmost(pt, pt.get_root());
        return astn;
    }
    else{
        throw std::runtime_error("Parse node root is not STMT, but should always be!");
    }
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
    else if (first_pn.gu.string_representation == "IF_STMT"){
        action_sequence.push_back(handle_IF_STMT(pt, first_pn));
    }
    else if (first_pn.gu.string_representation == ";"){

    }
    else if (first_pn.gu.string_representation == "VAR_DECL"){
        action_sequence.push_back(handle_VAR_DECL(pt, first_pn));
    }
    else if (first_pn.gu.string_representation == "CONST_DECL"){
        action_sequence.push_back(handle_CONST_DECL(pt, first_pn));
    }
    else if (first_pn.gu.string_representation == "ARR_DECL"){
        action_sequence.push_back(handle_ARR_DECL(pt, first_pn));
    }
    else if (first_pn.gu.string_representation == "F_DECL"){
        action_sequence.push_back(handle_F_DECL(pt, first_pn));
    }
    else if (first_pn.gu.string_representation == "F_DECL_OVERLOAD"){
        parse_node *node = (parse_node*)&first_pn;
        action_sequence.push_back(handle_F_DECL_OVERLOAD(pt, *node));
    }
    else if (first_pn.gu.string_representation == "STRUCT_DECL"){
        // TODO
    }
    else if (first_pn.gu.string_representation == "RETURN_STMT"){
        // TODO
    }
    else if (first_pn.gu.string_representation == "WHILE_STMT"){
        // TODO
    }
    else if (first_pn.gu.string_representation == "FOR_STMT"){
        // TODO
    }
    else{
        throw std::runtime_error("ERROR IN STMT");
    }

    handle_STMT(pt, pt.get_node_const(current_node_pn.children[0]), action_sequence);
}

ASTN handle_F_DECL(parse_tree const& pt, parse_node const& cn){
    ASTN node = new_ASTN(F_DECL);

    const auto& f_name = pt.get_node_const(cn.children[6]);
    const auto& f_input = pt.get_node_const(cn.children[4]);
    const auto& f_output = pt.get_node_const(cn.children[2]);
    const auto& f_body = pt.get_node_const(cn.children[0]);

    ASTN f_name_astn = get_identifier(pt, f_name);
    ASTN f_input_astn = handle_F_INPUT(pt, f_input);
    ASTN f_output_astn = handle_F_OUTPUT(pt, f_output);
    ASTN f_body_astn = handle_BLOCK_SEGMENT(pt, f_body);

    node->add_child(f_name_astn);
    node->add_child(f_input_astn);
    node->add_child(f_output_astn);
    node->add_child(f_body_astn);

    return node;
}

ASTN handle_F_DECL_OVERLOAD(parse_tree const& pt, parse_node & cn){
    const auto& f_decl_ifself = pt.get_node_const(cn.children[0]);
    ASTN node = handle_F_DECL(pt, f_decl_ifself);
    node->node_type = F_DECL_OVERLOAD;
    return node;
}

ASTN handle_F_OUTPUT(parse_tree const& pt, parse_node const& cn){
    ASTN node = new_ASTN(F_OUTPUT);
    const auto& first_pn = pt.get_node_const(cn.children[0]);

    if (first_pn.gu.string_representation == "VAR_SIGN"){
        ASTN var_sign = get_variable_signature(pt, first_pn);
        node->add_child(var_sign);
    }
    else if (first_pn.gu.string_representation == "ARR_SIGN"){
        ASTN arr_sign = handle_ARR_SIGN(pt, first_pn);
        node->add_child(arr_sign);
    }
    else if (first_pn.gu.string_representation == "VOID_TYPE"){
        return new_ASTN(F_OUTPUT_VOID);
    }
    else{
        throw std::runtime_error("Failed to parse F_OUTPUT");
    }
    return node;
}

ASTN handle_F_INPUT(parse_tree const& pt, parse_node const& cn){
    std::vector<ASTN> all_args;
    ASTN node = new_ASTN(F_INPUT);
    const auto& second_pn = pt.get_node_const(cn.children[1]);
    const auto& first_pn = pt.get_node_const(cn.children[0]);

    if (first_pn.gu.string_representation == "ARG_DECL"){
        all_args.push_back(handle_ARG_DECL(pt, second_pn));
    }
    else if (first_pn.gu.string_representation == "VOID_TYPE"){
        return new_ASTN(F_INPUT_VOID);
    }
    else {
        throw std::runtime_error("Failed to parse F_INPUT");
    }

    handle_OPT_ARG_DECL(pt, pt.get_node_const(cn.children[0]), all_args);

    for (auto &el : all_args){
        node->add_child(el);
    }

    return node;
}

ASTN handle_ARG_DECL(parse_tree const& pt, parse_node const& cn){
    ASTN arg = new_ASTN(F_ARG);
    const auto& first_pn = pt.get_node_const(cn.children[1]);
    const auto& identifier_pn = pt.get_node_const(cn.children[0]);

    if (first_pn.gu.string_representation == "VAR_SIGN"){
        ASTN var_sign = get_variable_signature(pt, first_pn);
        arg->add_child(var_sign);
    }
    else if (first_pn.gu.string_representation == "ARR_SIGN"){
        ASTN arr_sign = handle_ARR_SIGN(pt, first_pn);
        arg->add_child(arr_sign);
    }

    ASTN iden_pn = get_identifier(pt, identifier_pn);
    arg->add_child(iden_pn);

    return arg;
}

ASTN handle_OPT_ARG_DECL(parse_tree const& pt, parse_node const& cn,
                         std::vector<ASTN> &all_args){
    const auto& arg_decl = pt.get_node_const(cn.children[1]);
    const auto& next_arg_decl = pt.get_node_const(cn.children[0]);

    all_args.push_back(handle_ARG_DECL(pt, arg_decl));
    if (!next_arg_decl.children.empty()){
        handle_OPT_ARG_DECL(pt, next_arg_decl, all_args);
    }
}

ASTN handle_ARR_DECL(parse_tree const& pt, parse_node const& cn){
    ASTN node = new_ASTN(ARR_DECL);

    const auto& arr_sign = pt.get_node_const(cn.children.back());
    const auto& identifier = pt.get_node_const(cn.children[0]);

    ASTN arr_sign_astn = handle_ARR_SIGN(pt, arr_sign);
    ASTN indentifier_astn = get_identifier(pt, identifier);

    node->add_child(arr_sign_astn);
    node->add_child(indentifier_astn);

    return node;
}

ASTN get_identifier(parse_tree const& pt, parse_node const& cn){
    ASTN node = new_ASTN(AST_JUST_TEXT);
    node->optional_value.emplace();
    node->optional_value.value().value = cn.optional_token->attribute;
    return node;
}

ASTN handle_ARR_SIGN(parse_tree const& pt, parse_node const& cn){
    ASTN node = new_ASTN(ARR_SIGNATURE);

    const auto& dim_decl = pt.get_node_const(cn.children[2]);
    const auto& var_sign = pt.get_node_const(cn.children[0]);

    std::optional<ASTN> opt_dim_decl_astn = handle_OPT_DIM_DECL(pt, dim_decl);
    if (opt_dim_decl_astn.has_value()){
         node->add_child(opt_dim_decl_astn.value());
    }

    std::optional<ASTN> opt_var_sign_astn = get_variable_signature(pt, var_sign);
    if (opt_var_sign_astn.has_value()){
        node->add_child(opt_var_sign_astn.value());
    }

    return node;
}

std::optional<ASTN> handle_OPT_DIM_DECL(parse_tree const& pt, parse_node const& cn){
    if (cn.children.empty()) return {};
    return { handle_DIM_DECL(pt, pt.get_node_const(cn.children[0])) };
}

ASTN handle_DIM_DECL(parse_tree const& pt, parse_node const& cn){
    ASTN node = new_ASTN(DIM_DECL);
    std::vector<ASTN> dim_group;
    const auto& dim = pt.get_node_const(cn.children[1]);
    dim_group.push_back(handle_AEs(pt, dim));
    handle_NEXT_DIM_DECL(pt, pt.get_node_const(cn.children[0]), dim_group);

    for (auto& el : dim_group){
        node->add_child(el);
    }

    return node;
}

void handle_NEXT_DIM_DECL(parse_tree const& pt, parse_node const& cn,
                          std::vector<ASTN> &dim_group){
    if (cn.children.empty()) return;
    else{
        dim_group.push_back(handle_AEs(pt, pt.get_node_const(cn.children[1])));
        handle_NEXT_DIM_DECL(pt, pt.get_node_const(cn.children[0]), dim_group);
    }
}

ASTN handle_VAR_DECL(parse_tree const& pt, parse_node const& current_node_pn){
    ASTN var_decl = new_ASTN(VAR_DECL);
    std::vector<ASTN> var_decls;
    var_decls.push_back(var_decl);

    const auto& var_sign_pn = pt.get_node_const(current_node_pn.children[3]);
    const auto& identifier = pt.get_node_const(current_node_pn.children[2]);
    const auto& opt_complex_assignment = pt.get_node_const(current_node_pn.children[1]);
    const auto& opt_next_var_decl = pt.get_node_const(current_node_pn.children[0]);

    ASTN var_sign = get_variable_signature(pt, var_sign_pn);
    var_decl->add_child(var_sign);
    ASTN var_identifier = new_ASTN(AST_JUST_TEXT);
    var_identifier->optional_value.emplace();
    var_identifier->optional_value.value().value = identifier.optional_token->attribute;
    var_decl->add_child(var_identifier);
    std::optional<ASTN> opt_assignment = handle_opt_complex_assignment(pt, opt_complex_assignment);
    if (opt_assignment.has_value()){
        var_decl->add_child(opt_assignment.value());
    }
    std::optional<ASTN> next_var = handle_OPT_VAR_NEXT(pt, opt_next_var_decl, var_sign);
    if (next_var.has_value()){
        var_decl->add_child(next_var.value());
    }
    return var_decl;

}

std::optional<ASTN> handle_OPT_VAR_NEXT(parse_tree const& pt, parse_node const& current_node_pn, ASTN &var_type){
    if (current_node_pn.children.empty()) return {};

    ASTN var_decl = new_ASTN(VAR_DECL);
    std::vector<ASTN> var_decls;

    const auto& identifier = pt.get_node_const(current_node_pn.children[2]);
    const auto& opt_complex_assignment = pt.get_node_const(current_node_pn.children[1]);
    const auto& opt_next_var_decl = pt.get_node_const(current_node_pn.children[0]);

    ASTN &var_sign = var_type;
    var_decl->add_child(var_sign);
    ASTN var_identifier = new_ASTN(AST_JUST_TEXT);
    var_identifier->optional_value.emplace();
    var_identifier->optional_value.value().value = identifier.optional_token->attribute;
    var_decl->add_child(var_identifier);
    std::optional<ASTN> opt_assignment = handle_opt_complex_assignment(pt, opt_complex_assignment);
    if (opt_assignment.has_value()){
        var_decl->add_child(opt_assignment.value());
    }
    std::optional<ASTN> next_var = handle_OPT_VAR_NEXT(pt, opt_next_var_decl, var_sign);
    if (next_var.has_value()){
        var_decl->add_child(next_var.value());
    }
    return var_decl;
}

std::optional<ASTN> handle_opt_complex_assignment(parse_tree const& pt, parse_node const& cn){
    if (cn.children.empty()) return {};
    return { handle_complex_assignment(pt, pt.get_node_const(cn.children[0])) };
}

ASTN handle_complex_assignment(parse_tree const& pt, parse_node const& cn){
    return handle_AEs(pt, pt.get_node_const(cn.children[0]));
}

ASTN get_variable_signature(parse_tree const& pt, parse_node const& current_node_pn){
    ASTN var_sign = new_ASTN(VARIABLE_SIGNATURE);
    auto const& opt_type_mod_pn = pt.get_node_const(current_node_pn.children.back());
    auto const& type_spec_pn = pt.get_node_const(current_node_pn.children[0]);

    if (!opt_type_mod_pn.children.empty()){
        ASTN no_type_mod = new_ASTN(MOD_TYPE_EMPTY);
        var_sign->add_child(no_type_mod);
    }
    else{
        ASTN type_mod = new_ASTN(MOD_TYPE_UNSIGNED);
    }

    ASTN var_type = new_ASTN(TYPE_SPEC);
    var_type->optional_value.emplace();
    var_type->optional_value.value().value = { pt.get_node_const(type_spec_pn.children[0]).gu.string_representation };
    var_sign->add_child(var_type);

    return var_sign;
}

ASTN handle_CONST_DECL(parse_tree const& pt, parse_node const& cn){
    ASTN var_decl = new_ASTN(CONST_DECL);

    const auto& var_sign_pn = pt.get_node_const(cn.children[2]);
    const auto& identifier = pt.get_node_const(cn.children[1]);
    const auto& complex_assignment = pt.get_node_const(cn.children[0]);
    ASTN var_sign = get_variable_signature(pt, var_sign_pn);
    var_decl->add_child(var_sign);
    ASTN var_identifier = new_ASTN(AST_JUST_TEXT);
    var_identifier->optional_value.emplace();
    var_identifier->optional_value.value().value = identifier.optional_token->attribute;
    var_decl->add_child(var_identifier);
    ASTN complex_assignment_astn = handle_complex_assignment(pt, complex_assignment);
    var_decl->add_child(complex_assignment_astn);

    return var_decl;
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

ASTN handle_IF_STMT(parse_tree const& pt, parse_node const& current_node_pn){
    ASTN node = new_ASTN(IF);

    std::vector<std::shared_ptr<ast_node>> elifs_gathered;
    const auto& condition_pn = pt.get_node_const(current_node_pn.children[4]);
    const auto& exec_if_true_pn = pt.get_node_const(current_node_pn.children[2]);
    const auto& elif_start_pn = pt.get_node_const(current_node_pn.children[1]);
    const auto& opt_else_pn = pt.get_node_const(current_node_pn.children[0]);

    ASTN condition = handle_AEs(pt, condition_pn);
    ASTN exec_if_true = handle_BLOCK_SEGMENT(pt, exec_if_true_pn);
    handle_ELIF_recursive(pt, elif_start_pn, elifs_gathered);

    node->add_child(condition);
    node->add_child(exec_if_true);

    for (ASTN &astn : elifs_gathered){
        node->add_child(astn);
    }

    if (!opt_else_pn.children.empty()){
        ASTN exec_if_false = handle_BLOCK_SEGMENT(pt, pt.get_node_const(opt_else_pn.children[0]));
        node->children.push_back(exec_if_false);
    }

    return node;
}

ASTN handle_BLOCK_SEGMENT(parse_tree const& pt, parse_node const& current_node_pn){
    ASTN node = new_ASTN(STATEMENT_SEQUENCE);
    std::vector<ASTN> action_sequence;
    handle_STMT(pt, pt.get_node_const(current_node_pn.children[1]), action_sequence);

    for (ASTN &astn : action_sequence){
        node->add_child(astn);
    }

    return node;
}

void handle_ELIF_recursive(parse_tree const& pt, parse_node const& current_node_pn,
                    std::vector<ASTN> &elifs_gathered){
    if (current_node_pn.children.empty()) return;
    const auto& condition_pn = pt.get_node_const(current_node_pn.children[3]);
    const auto& exec_if_true_pn = pt.get_node_const(current_node_pn.children[1]);
    const auto& next_elif_pn = pt.get_node_const(current_node_pn.children[0]);

    ASTN elif_astn = new_ASTN(ELIF);
    auto condition_astn = handle_AEs(pt, condition_pn);
    auto exec_if_true_astn = handle_BLOCK_SEGMENT(pt, exec_if_true_pn);
    elif_astn->add_child(condition_astn);
    elif_astn->add_child(exec_if_true_astn);

    elifs_gathered.push_back(elif_astn);

    handle_ELIF_recursive(pt, next_elif_pn, elifs_gathered);
}

std::shared_ptr<ast_node> handle_AEs(parse_tree const& pt, parse_node const& current_node_pn){
    auto const& factor_pn = pt.get_node_const(current_node_pn.children.back());
    auto const& AE_pn = pt.get_node_const(current_node_pn.children[0]);

    std::vector<std::shared_ptr<ast_node>> arithmetic_expression_parsed;

    if (pt.get_node_const(factor_pn.children.back()).gu.string_representation == "("){
        handle_PARANT_AE(pt, factor_pn, arithmetic_expression_parsed);
    }
    else{
        arithmetic_expression_parsed.push_back(handle_F(pt, factor_pn));
    }

    handle_AE(pt, AE_pn, arithmetic_expression_parsed);

    AE_parser parser { arithmetic_expression_parsed };
    return parser.convert_to_ast();
}

void handle_AEs_nonresolving(parse_tree const& pt, parse_node const& current_node_pn,
                                                  std::vector<std::shared_ptr<ast_node>> &arithmetic_expression_parsed){
    auto const& factor_pn = pt.get_node_const(current_node_pn.children.back());
    auto const& AE_pn = pt.get_node_const(current_node_pn.children[0]);

    if (pt.get_node_const(factor_pn.children.back()).gu.string_representation == "("){
        handle_PARANT_AE(pt, factor_pn, arithmetic_expression_parsed);
    }
    else{
        arithmetic_expression_parsed.push_back(handle_F(pt, factor_pn));
    }

    handle_AE(pt, AE_pn, arithmetic_expression_parsed);
}

void handle_PARANT_AE(parse_tree const& pt, parse_node const& current_node_pn, std::vector<std::shared_ptr<ast_node>> &arithmetic_expression_parsed){
    auto paran_open = std::make_shared<ast_node>(PAR_OPEN);

    arithmetic_expression_parsed.push_back(paran_open);
    handle_AEs_nonresolving(pt, pt.get_node_const(current_node_pn.children[1]), arithmetic_expression_parsed);

    auto paran_close = std::make_shared<ast_node>(PAR_CLOSE);
    arithmetic_expression_parsed.push_back(paran_close);
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

    if (pt.get_node_const(factor_pn.children.back()).gu.string_representation == "("){
        handle_PARANT_AE(pt, factor_pn, arithmetic_expression_parsed);
    }
    else{
        arithmetic_expression_parsed.push_back(handle_F(pt, factor_pn));
    }

    handle_AE(pt, recursive_next_AE_pn, arithmetic_expression_parsed);
}

std::shared_ptr<ast_node> handle_F(parse_tree const& pt, parse_node const& current_node_pn){
    const auto& opt_unary_op = pt.get_node_const(current_node_pn.children.back());
    const auto& f_buffer_pn = pt.get_node_const(current_node_pn.children[0]);
    auto const& first_symbol_of_f_buffer_pn = pt.get_node_const(f_buffer_pn.children.back());

    std::shared_ptr<ast_node> factor = std::make_shared<ast_node>(AST_NODE_TYPE::AST_INVALID);

    factor->optional_value.emplace();

    // Assign unary operator;
    std::optional<UNARY_OPERATOR> unary_operator = handle_OPT_UNARY_OP(pt, opt_unary_op);
    if (unary_operator.has_value()) factor->optional_value.value().optional_unary_operator = unary_operator;

    if (first_symbol_of_f_buffer_pn.gu.string_representation == "F_IDENTIFIER"){
        factor->node_type = AST_NODE_TYPE::IDENTIFIER_AST;
        auto const& identifier_pn = pt.get_node_const(first_symbol_of_f_buffer_pn.children.back());
        factor->optional_value->value = identifier_pn.optional_token->attribute;
        std::vector<std::shared_ptr<ast_node>> trailing_operators;
        handle_OPT_F_TRAIL(pt, pt.get_node_const(current_node_pn.children[1]), trailing_operators);
        for (auto& trailing_op : trailing_operators){
            factor->add_child(trailing_op);
        }
    }
    else if (first_symbol_of_f_buffer_pn.gu.string_representation == "F_NUMBER"){
        const auto& number_pn = pt.get_node_const(first_symbol_of_f_buffer_pn.children.back());
        if (number_pn.optional_token->type == TOKEN_TYPE::CONSTANT_INTEGER){
            factor->node_type = VAL_INT;
        }
        else if (number_pn.optional_token->type == TOKEN_TYPE::CONSTANT_FP){
            factor->node_type = VAL_FP;
        }
        else{
            throw std::exception{};
        }
        factor->optional_value.value().value = number_pn.optional_token->attribute;
    }
    else if (first_symbol_of_f_buffer_pn.gu.string_representation == "F_STRING"){
        const auto& string_pn = pt.get_node_const(first_symbol_of_f_buffer_pn.children.back());
        factor->node_type = VAL_STRING;
        factor->optional_value.value().value = string_pn.optional_token->attribute;
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