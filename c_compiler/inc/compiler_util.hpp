#ifndef compiler_util_hpp
#define compiler_util_hpp

#include "../../common/inc/ast.hpp"
#include "../../common/inc/util.hpp"

class CompilerUtil {
 public:
  static std::string makeUniqueId(const std::string& base_id);

  // Count number of bytes that will be used by a function.
  // Increase on:
  // - integer declaration: + 4 bytes.
  static int countBytesForDeclarationsInFunction(const Node* ast_node);

};

#endif