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
#include "utility/scoped_timer.h"
#include <unistd.h>

int main() {
    {
        scoped_timer timer { "Compilation took: " };
        compile("../test_source.pbl");
    }
    return 0;
}

