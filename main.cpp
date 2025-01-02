#include "output.hpp"
#include "nodes.hpp"
#include "SemanticAnalyzer.hpp"

// Extern from the bison-generated parser
extern int yyparse();

extern std::shared_ptr<ast::Node> program;

int main() {
    // Parse the input. The result is stored in the global variable `program`
    yyparse();
    //std::cout << "parse done" << std::endl;
    // Print the AST using the PrintVisitor
    SemanticAnalyzer sa;
    program->accept(sa);


}
