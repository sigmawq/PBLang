#ifdef DEBUG_MODE
    #define DEBUG_LOG(msg) std::cout << msg << std::endl
#else
    #define DEBUG_LOG(msg)
#endif

#include "ast/abstract_syntax_tree.h"
#include "parse_operations.h"
#include "utility/pbl_utility.h"
#include "ast/ast_node.h"
#include "ast/ast_converter.h"

#include "compile.h"

int main() {
    compile("../test_source.pbl");
    return 0;
}

