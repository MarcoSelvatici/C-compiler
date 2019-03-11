#ifndef compiler_util_hpp
#define compiler_util_hpp

#include "../../common/inc/ast.hpp"
#include "../../common/inc/util.hpp"

#include <unordered_map>

class CompilerUtil {
 private:
  static void extractArgumentNames(const ArgumentListNode* argument_list_node,
                                   std::vector<std::string>& argument_names);

 public:
  static const std::string NO_ARGUMENT;
  static std::string makeUniqueId(const std::string& base_id);

  // Count number of bytes that will be used by a function.
  // Increase on:
  // - integer declaration: + 4 bytes.
  static int countBytesForDeclarationsInFunction(const Node* ast_node);

  // Returns a vector of strings containing the name of the arguments of a function.
  // The vector has size of at least 4.
  // If there are less than four argument, a "?" is returned to signify that is not a
  // parameter.
  static std::vector<std::string> getArgumentNamesFromFunctionDeclaration(
    const ArgumentListNode* argument_list_node);
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
  // Returns a vector of temporary registers currently used.
  std::vector<std::string> get_temporary_registers_in_use();
};

class FunctionContext {
 private:
  std::unordered_map<std::string, int> variable_to_offset_in_stack_frame_;
  std::unordered_map<int, std::string> offset_in_stack_frame_to_variable_;
  std::vector<std::string> loop_labels_;
  std::string epilogue_label_;
  int frame_size_; // In bytes.
  const int word_length_ = 4;
  const int call_arguments_size_ = 4 * word_length_; // 4 words.

 public:
  FunctionContext(int frame_size, const std::string& epilogue_label);

  const std::string& getEpilogueLabel() const;
  
  // Record loop labels in case of a break/continue statement.
  const std::string& getStartLoopLabel() const;
  const std::string& getEndLoopLabel() const;
  void saveLoopLabels(const std::string& start_loop_label, 
                      const std::string& end_loop_label);
  void removeLoopLabels();

  // Record the stack offset for a variable in the current stack frame. 
  int placeVariableInStack(const std::string& var_name);
  // Get the stack offset for a variable in the current stack frame.
  int getOffsetForVariable(const std::string& var_name);

  // Save the offset for an argument. Note that these are stored in the stack frame of the
  // previous function. In fact, the passed offset must be >= frame_size.
  void saveOffsetForArgument(const std::string& arg_name, int offset);
};

#endif