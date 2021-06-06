#define DEBUG_MODE
#ifdef DEBUG_MODE
    #define DEBUG_LOG(msg) std::cout << msg << std::endl
#else
    #define DEBUG_LOG(msg)
#endif

#include "source/parse_operations.h"
#include "source/utility/pbl_utility.h"
#include "source/ast/ast_node.h"
#include "source/ast/ast_converter.h"

#include "source/compile.h"
#include "source/utility/scoped_timer.h"
#include <unistd.h>

int main(int argc, char* argv[]) {
    {
        try {
            if (argc < 2) { throw std::runtime_error("No source file specified"); }
            scoped_timer timer { "Compilation took: " };
            std::string source { argv[1] };
            compile(source);
        }
        catch (std::runtime_error &err) {
            std::cout << "Compilation error: " << err.what() << std::endl;
        }
    }
    return 0;
}

