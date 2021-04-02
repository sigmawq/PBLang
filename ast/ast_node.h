#ifndef LEXER_TEST_AST_NODE_H
#define LEXER_TEST_AST_NODE_H

#include "../parse_tree.h"

enum AST_NODE_TYPE{
    // Program root
        PROGRAM_ROOT,

    // For error checking
        AST_INVALID,

    // Math

        // Parenthesis open/close
        PAR_OPEN, PAR_CLOSE,

        // ** FACTOR **

        // Values
        VAL_INT, VAL_FP,

        // Function call
        F_CALL, F_CALL_OVERLOAD,

        // Array access
        ARRAY_ACCESS,

        // Struct access
        STRUCT_ACCESS,

        // Operators (not unary
        // Higher operator has higher priority
        POW, MUL, DIV, ADD, SUB
};

enum UNARY_OPERATOR{
    PLUS, MINUS, CAST, LOGICAL_NOT
};

struct ast_value{
    std::optional<UNARY_OPERATOR> optional_unary_operator;
    std::string value;
};

struct ast_node{
    AST_NODE_TYPE node_type;
    std::optional<ast_value> optional_value; // AST_node MAY not have a value (example: if statement node will have just a node_type, value is relevant only for some things)
    std::vector<std::reference_wrapper<ast_node>> children;

    void add_child(std::reference_wrapper<ast_node> new_child) {
        children.push_back(new_child);
    }

    void add_child_front(std::reference_wrapper<ast_node> new_child){
        children.insert(children.begin(), new_child);
    }
    bool is_operator() const { return node_type >= POW && node_type <= SUB; }
    bool is_factor() const { return node_type >= VAL_INT && node_type <= STRUCT_ACCESS; }

    std::string to_string(){
        std::string str;
        pbl_utility::str_compose(str, "AST | TYPE: ", std::to_string(this->node_type));
        if (optional_value.has_value()){
            pbl_utility::str_compose(str, " | VALUE: ");
            if (optional_value.value().optional_unary_operator.has_value()){
                pbl_utility::str_compose(str, "[UNARY OP TYPE: ",std::to_string(optional_value.value().optional_unary_operator.value()), "], ");
            }
            pbl_utility::str_compose(str, optional_value.value().value);
        }
        return str;
    }

private:
    static void R_to_string_recursise(std::string &str, ast_node &node, int tabs, int tabs_per_layer) {
        pbl_utility::str_compose(str, tabs_get(tabs), node.to_string(), '\n');
        tabs += tabs_per_layer;
        for (int i = 0; i < node.children.size(); i++){
            R_to_string_recursise(str, node.children[i], tabs, tabs_per_layer);
        }
    }
public:

    std::string to_string_recursive(int tabs = 0, int tabs_per_layer = 4) {
        std::string str;
        R_to_string_recursise(str, *this, tabs, tabs_per_layer);
        return str;
    }
};

#endif //LEXER_TEST_AST_NODE_H
