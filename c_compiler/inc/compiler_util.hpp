#ifndef compiler_util_hpp
#define compiler_util_hpp

#include "../../common/inc/ast.hpp"
#include "../../common/inc/util.hpp"

namespace cu {

unsigned int unique_id_counter = 0;

std::string makeUniqueId(const std::string& base_id);

// Count number of bytes that will be used by a function.
// Increase on:
// - integer declaration: + 4 bytes.
int countBytesForDeclarationsInFunction(const FunctionDefinition* function_definition);

} // End namespace.

#endif