#ifndef compiler_util_hpp
#define compiler_util_hpp

#include "../../common/inc/ast.hpp"
#include "../../common/inc/util.hpp"

#include <unordered_map>

class CompilerUtil {
 public:
  static std::string makeUniqueId(const std::string& base_id);

  // Count number of bytes that will be used by a function.
  // Increase on:
  // - integer declaration: + 4 bytes.
  static int countBytesForDeclarationsInFunction(const Node* ast_node);

};

class RegisterAllocator {
 private:
  // From $t0 to $t7.
  std::vector<bool> tmp_reg_used_;
  const int tmp_reg_size_ = 8;
 
 public:
  RegisterAllocator();

  // Returns a free register. Must be freed at the end of the usage.
  std::string requestFreeRegister();
  // Make a register available for new allocation.
  void freeRegister(const std::string& reg);

};

class FunctionContext {
 private:
  std::unordered_map<std::string, int> variable_to_offset_in_stack_frame_;
  std::unordered_map<int, std::string> offset_in_stack_frame_to_variable_;
  int frame_size_; // In bytes.
  const int word_length_ = 4;
  const int call_arguments_size_ = 4 * word_length_; // 4 words.

 public:
  FunctionContext(int frame_size);

  // Record the stack offset for a variable in the current stack frame. 
  int placeVariableInStack(const std::string& var_name);
  // Get the stack offset for a variable in the current stack frame.
  int getOffsetForVariable(const std::string& var_name);

};

#endif