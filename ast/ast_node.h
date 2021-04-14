#ifndef LEXER_TEST_AST_NODE_H
#define LEXER_TEST_AST_NODE_H

#include "../parse_tree.h"
#include "../utility/pbl_utility.h"

enum AST_NODE_TYPE{
    // Program root
        PROGRAM_ROOT,

    // For error checking
        AST_INVALID,

    // Math

        // Parenthesis open/close
        PAR_OPEN, PAR_CLOSE,

        // ** FACTOR **
        __FACTORSTART__,

        // Values
        VAL_INT, VAL_FP,

        // Function call
        F_CALL, F_CALL_OVERLOAD,

        // Array access
        ARRAY_ACCESS,

        // Struct access
        STRUCT_ACCESS,

        __FACTOREND__,

        // Operators (not unary)
        // Greater operator has higher priority
        _OPERATOR_START_,
        EQUALS,

        COMP_EQUALS, COMP_GREATER_EQUALS, COMP_GREATER, COMP_LESS_EQUALS, COMP_LESS,

        LOGICAL_OR, LOGICAL_AND,

        POW, MUL, DIV, ADD, SUB,

        __OPERATOREND__,

        // Flow control
        IF, FOR, WHILE
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
    std::vector<std::shared_ptr<ast_node>> children;

    void add_child(std::shared_ptr<ast_node> &new_child) {
        children.emplace_back(new_child);
    }

    void add_child_front(std::shared_ptr<ast_node> &new_child){
        children.insert(children.begin(), new_child);
    }
    [[nodiscard]] bool is_operator() const { return node_type >= _OPERATOR_START_ && node_type <= __OPERATOREND__; }
    [[nodiscard]] bool is_factor() const { return node_type >= __FACTORSTART__ && node_type <= __FACTOREND__; }

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
        pbl_utility::str_compose(str, pbl_utility::tabs_get(tabs), node.to_string(), '\n');
        tabs += tabs_per_layer;
        for (int i = 0; i < node.children.size(); i++){
            R_to_string_recursise(str, *node.children[i], tabs, tabs_per_layer);
        }
    }
public:

    std::string to_string_recursive(int tabs = 0, int tabs_per_layer = 4) {
        std::string str;
        R_to_string_recursise(str, *this, tabs, tabs_per_layer);
        return str;
    }

    ast_node(AST_NODE_TYPE type) : node_type(type) {}
};

#endif //LEXER_TEST_AST_NODE_H