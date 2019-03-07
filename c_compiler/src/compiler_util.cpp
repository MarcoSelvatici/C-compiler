#include "../inc/compiler_util.hpp"

using namespace cu;

std::string makeUniqueId(const std::string& base_id) {
  return "_" + base_id + "_" + std::to_string(unique_id_counter++);
}

int countBytesForDeclarationsInFunction(const FunctionDefinition* function_definition) {
  // TODO.
  return 400;
}
