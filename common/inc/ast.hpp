#ifndef ast_hpp
#define ast_hpp

// Include headers in ast that contains the classes for the tree nodes.
#include "ast/ast_expression.hpp"
#include "ast/ast_primitives.hpp"

#include <vector>

extern std::vector<const Expression*> parseAST();
extern void yyset_in(FILE* fd);

#endif