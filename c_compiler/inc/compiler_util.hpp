#ifndef compiler_util_hpp
#define compiler_util_hpp

#include "../../common/inc/ast.hpp"
#include "../../common/inc/util.hpp"

#include <unordered_map>
#include <unordered_set>
#include <stack>

class PairHash {
 public:
  template <class T1, class T2>
  std::size_t operator () (const std::pair<T1,T2> &p) const {
    auto h1 = std::hash<T1>{}(p.first);
    auto h2 = std::hash<T2>{}(p.second);

    return h1 ^ h2;
  }
};

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
  
  // Evaluates a constant expression of integer.
  static int evaluateConstantExpression(const Node* expr);
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
  std::vector<std::string> getTemporaryRegistersInUse();
};

class FunctionContext {
 private:
  std::unordered_map<std::pair<std::string, std::string>, int, PairHash> variable_to_offset_in_stack_frame_;
  std::unordered_map<int, std::pair<std::string, std::string>> offset_in_stack_frame_to_variable_;
  std::vector<std::string> scopes_list_;
  std::stack<std::string> break_labels_;
  std::stack<std::string> continue_labels_;
  std::stack<std::string> default_labels_;
  std::string function_epilogue_label_;
  int frame_size_; // In bytes.
  const int word_length_ = 4;
  const int call_arguments_size_ = 4 * word_length_; // 4 words.

 public:
  FunctionContext(int frame_size, const std::string& function_epilogue_label);

  const std::string& getFunctionEpilogueLabel() const;
  
  // Record loop labels in case of a break/continue statement.
  const std::string& getBreakLabel() const;
  const std::string& getContinueLabel() const;
  const std::string& getDefaultLabel() const;
  void insertWhileLabels(const std::string& continue_label, 
                         const std::string& break_label);
  void removeWhileLabels();
  void insertForLabels(const std::string& continue_label, const std::string& break_label);
  void removeForLabels();
  void insertSwitchLabels(const std::string& default_label, 
                          const std::string& break_label);
  void removeSwitchLabels();
  void insertScope(const std::string& scope_id);
  void removeScope();

  // Record the offset for a variable in the current stack frame. 
  int placeVariableInStack(const std::string& var_name, const std::string& scope_id,
                           const bool& is_declaration);
  // Get the offset for a variable in the current stack frame.
  int getOffsetForVariable(const std::string& var_name);

  // Save the offset for an argument. Note that these are stored in the stack frame of the
  // previous function. In fact, the passed offset must be >= frame_size.
  void saveOffsetForArgument(const std::string& arg_name, int offset, 
                             const std::string& scope_id);

  // Reserve space in memory for the array.
  // Every position reserved for the array is named as: 'array_name@index' where index
  // goes from zero to size - 1. 
  void reserveSpaceForArray(const std::string& array_name, int size,
                            const std::string& scope_id);

  // Get the base offset for an array in the current stack frame.
  int getBaseOffsetForArray(const std::string& array_name);

  bool isLocalVariable(const std::string& id);
};

class GlobalVariables {
 private:
  // Maps global variables id to their info. E.g.:
  // "a" --> "normal"    (say declared as: int a = 2;)
  // "b" --> "array"
  std::unordered_map<std::string, std::string> id_to_info_;
 
 public:
  void addNewGlobalVariable(const std::string& id, const std::string& info);

  bool isGlobalVariable(const std::string& id) const;

  const std::string& getInfoForVariable(const std::string& id) const;

  const std::vector<std::string> getAllGlobalVariableIds() const;
};

class FunctionDeclarations {
 private:
  std::unordered_set<std::string> ids_;
 
 public:
  bool isIdOfDeclaredOnlyFunction(const std::string& id);

  void insertId(const std::string& id);
};

#endif