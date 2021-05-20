#ifndef LEXER_TEST_AST_NODE_H
#define LEXER_TEST_AST_NODE_H

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

#include "../parse_tree.h"
#include "../utility/pbl_utility.h"


enum AST_NODE_TYPE{
        AST_INVALID,
        AST_JUST_TEXT,
        STATEMENT_SEQUENCE,
        // Parenthesis open/close
        PAR_OPEN, PAR_CLOSE,

        // ** FACTOR **
        __FACTORSTART__,

        // Values
        IDENTIFIER_AST, VAL_INT, VAL_FP, VAL_STRING,

        F_CALL, F_CALL_OVERLOAD,
        ARRAY_ACCESS,
        STRUCT_ACCESS,

        __FACTOREND__,

        // ** Operators ** (not unary)
        // Greater operator has higher priority
        _OPERATOR_START_,
        COMP_EQUALS, COMP_NOT_EQUALS, COMP_GREATER_EQUALS, COMP_GREATER, COMP_LESS_EQUALS, COMP_LESS,
        LOGICAL_OR, LOGICAL_AND,
        POW, MUL, DIV, ADD, SUB,
        EQUALS,

        __OPERATOREND__,

        IF, ELIF, FOR, WHILE,
        VAR_DECL, CONST_DECL, ARR_DECL,
        STRUCT_DECL, RETURN_STMT,
        F_DECL, F_DECL_OVERLOAD,
        VARIABLE_SIGNATURE,
        ARR_SIGNATURE,
        DIM_DECL,

        MOD_TYPE_UNSIGNED, MOD_TYPE_EMPTY,

        TYPE_SPEC,
        F_INPUT, F_OUTPUT, F_INPUT_VOID, F_OUTPUT_VOID,
        F_ARG

};

static std::string ast_enum_text(AST_NODE_TYPE type){
    std::string result;
    switch (type) {
        case WHILE: result = "WHILE"; break;
        case FOR: result = "FOR"; break;
        case ELIF: result = "ELIF"; break;
        case STATEMENT_SEQUENCE:
            result = "STATEMENT_SEQUENCE";
            break;
        case AST_INVALID:
            result = "AST_INVALID";
            break;
        case PAR_OPEN:
            result = "PAR_OPEN";
            break;
        case PAR_CLOSE:
            result = "PAR_CLOSE";
            break;
        case __FACTORSTART__:
            result = "__FACTORSTART__";
            break;
        case IDENTIFIER_AST:
            result = "IDENTIFIER_AST";
            break;
        case VAL_INT:
            result = "VAL_INT";
            break;
        case VAL_FP:
            result = "VAL_FP";
            break;
        case VAL_STRING:
            result = "VAL_STRING";
            break;
        case F_CALL:
            result = "F_CALL";
            break;
        case F_CALL_OVERLOAD:
            result = "F_CALL_OVERLOAD";
            break;
        case ARRAY_ACCESS:
            result = "ARRAY_ACCESS";
            break;
        case STRUCT_ACCESS:
            result = "STRUCT_ACCESS";
            break;
        case __FACTOREND__:
            result = "__FACTOREND__";
            break;
        case _OPERATOR_START_:
            result = "_OPERATOR_START_";
            break;
        case EQUALS:
            result = "EQUALS";
            break;
        case COMP_EQUALS:
            result = "COMP_EQUALS";
            break;
        case COMP_GREATER_EQUALS:
            result = "COMP_GREATER_EQUALS";
            break;
        case COMP_GREATER:
            result = "COMP_GREATER";
            break;
        case COMP_LESS_EQUALS:
            result = "COMP_LESS_EQUALS";
            break;
        case COMP_LESS:
            result = "COMP_LESS";
            break;
        case LOGICAL_OR:
            result = "LOGICAL_OR";
            break;
        case LOGICAL_AND:
            result = "LOGICAL_AND";
            break;
        case POW:
            result = "POW";
            break;
        case MUL:
            result = "MUL";
            break;
        case DIV:
            result = "DIV";
            break;
        case ADD:
            result = "ADD";
            break;
        case SUB:
            result = "SUB";
            break;
        case __OPERATOREND__:
            result = "__OPERATOREND__";
            break;
        case IF:
            result = "IF";
            break;
        case VAR_DECL:
            result = "VAR_DECL";
            break;
        case AST_JUST_TEXT:
            result = "AST_JUST_TEXT";
            break;
        case CONST_DECL:
            result = "CONST_DECL";
            break;
        case ARR_DECL:
            result = "ARR_DECL";
            break;
        case STRUCT_DECL:
            result = "STRUCT_DECL";
            break;
        case RETURN_STMT:
            result = "RETURN_STMT";
            break;
        case F_DECL:
            result = "F_DECL";
            break;
        case F_DECL_OVERLOAD:
            result = "F_DECL_OVERLOAD";
            break;
        case VARIABLE_SIGNATURE:
            result = "VARIABLE_SIGNATURE";
            break;
        case MOD_TYPE_UNSIGNED:
            result = "MOD_TYPE_UNSIGNED";
            break;
        case MOD_TYPE_EMPTY:
            result = "MOD_TYPE_EMPTY";
            break;
        case TYPE_SPEC:
            result = "TYPE_SPEC";
            break;
        case ARR_SIGNATURE:
            result = "ARR_SIGNATURE";
            break;
        case DIM_DECL:
            result = "DIM_DECL";
            break;
        case F_INPUT:
            result = "F_INPUT";
            break;
        case F_OUTPUT:
            result = "F_OUTPUT";
            break;
        case F_INPUT_VOID:
            result = "F_INPUT_VOID";
            break;
        case F_OUTPUT_VOID:
            result = "F_OUTPUT_VOID";
            break;
        case F_ARG:
            result = "F_ARG";
            break;
        case COMP_NOT_EQUALS:
            result = "COMP_NOT_EQUALS";
            break;
        default:
            throw std::runtime_error("Text for enum not defined");

    }
    return result;
}

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
    //virtual llvm::Value *codegen() = 0;
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
        pbl_utility::str_compose(str, ast_enum_text(this->node_type));
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