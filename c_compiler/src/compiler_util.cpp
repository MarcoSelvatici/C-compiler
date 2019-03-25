#include "../inc/compiler_util.hpp"

// CompilerUtil.

unsigned int unique_id_counter = 0;

const std::string CompilerUtil::NO_ARGUMENT = "?NO_FUNC_ARGUMENT";

std::string CompilerUtil::makeUniqueId(const std::string& base_id) {
  return "_" + base_id + "_" + std::to_string(unique_id_counter++);
}

int CompilerUtil::countBytesForDeclarationsInFunction(const Node* ast_node) {
  const std::string& node_type = ast_node->getType();
  if (node_type == "FunctionDefinition") {
    // Look into body.
    return countBytesForDeclarationsInFunction(
      dynamic_cast<const FunctionDefinition*>(ast_node)->getBody());
  }
  
  else if (node_type == "StatementListNode") {
    const StatementListNode* statement_list_node =
      dynamic_cast<const StatementListNode*>(ast_node);
    if(statement_list_node->isEmptyStatementList()) {
      return 0;
    } else if (!statement_list_node->hasNextStatement()) {
      return countBytesForDeclarationsInFunction(statement_list_node->getStatement());
    } else if (statement_list_node->hasNextStatement()) {
      return countBytesForDeclarationsInFunction(statement_list_node->getStatement()) +
             countBytesForDeclarationsInFunction(statement_list_node->getNextStatement());
    }
  }
  else if (node_type == "CompoundStatement") {
    const CompoundStatement* compound_statement =
      dynamic_cast<const CompoundStatement*>(ast_node);
    return countBytesForDeclarationsInFunction(compound_statement->getStatementList());
  }

  else if(node_type == "WhileStatement") {
    return countBytesForDeclarationsInFunction(
      dynamic_cast<const WhileStatement*>(ast_node)->getBody());
  }

  else if(node_type == "IfStatement") {
    const IfStatement* if_statement = dynamic_cast<const IfStatement*>(ast_node);
    if (!if_statement->hasElseBody()) {
      return countBytesForDeclarationsInFunction(if_statement->getIfBody());
    } else {
      return countBytesForDeclarationsInFunction(if_statement->getIfBody()) +
             countBytesForDeclarationsInFunction(if_statement->getElseBody());
    }
  }

  else if(node_type == "DeclarationExpressionList") {
    const DeclarationExpressionList* declaration_expression_list =
      dynamic_cast<const DeclarationExpressionList*>(ast_node);
    
    const std::string& type = declaration_expression_list->getTypeSpecifier();
    if (type == "int") {
      const DeclarationExpressionListNode* declaration_expression_list_node =
        dynamic_cast<const DeclarationExpressionListNode*>
        (declaration_expression_list->getDeclarationList());
      
      int total_bytes = 0;
      while (declaration_expression_list_node != nullptr){
        const Variable* variable =
          dynamic_cast<const Variable*>(declaration_expression_list_node->getVariable());
        if (variable->getInfo() == "normal" || variable->getInfo() == "pointer") {
          // Int is 4 bytes.
          total_bytes += 4;
        } else if (variable->getInfo() == "array") {
          total_bytes += evaluateConstantExpression(variable->getArrayIndexOrSize()) * 4;
        } else {
          if (Util::DEBUG) {
            std::cerr << "Unexpected variable type while determinig size of stack frame: "
                      << variable->getInfo() << "." << std::endl;
          }
          Util::abort();
        }

        if (declaration_expression_list_node->hasNext()) {
          declaration_expression_list_node = 
            dynamic_cast<const DeclarationExpressionListNode*>
            (declaration_expression_list_node->getNext());
        } else {
          break;
        }
      }
      return total_bytes;
    } else {
      if(Util::DEBUG) {
        std::cerr << "WARNING: Found non int variable while analyzing the AST to "
                  << "initialize function stack frame." << std::endl;
      }
      return 0;
    }
  }

  // Other node type that cannot have any declaration.
  else {
    return 0;
  }
}

void CompilerUtil::extractArgumentNames(const ArgumentListNode* argument_list_node,
                                        std::vector<std::string>& argument_names) {
  if (argument_list_node->isEmptyArgumentList()) {
    return;
  }
  else if (!argument_list_node->hasNextArgument()) {
    // Last argument.
    const DeclarationExpressionList* argument =
      dynamic_cast<const DeclarationExpressionList*>(argument_list_node->getArgument());
    const DeclarationExpressionListNode* argument_declaration =
      dynamic_cast<const DeclarationExpressionListNode*>(argument->getDeclarationList());
    
    const std::string& variable_id =
      (dynamic_cast<const Variable*>(argument_declaration->getVariable()))->getId();
    argument_names.push_back(variable_id);
  }
  else if (argument_list_node->hasNextArgument()) {
    // More arguments.
    const DeclarationExpressionList* argument =
      dynamic_cast<const DeclarationExpressionList*>(argument_list_node->getArgument());
    const DeclarationExpressionListNode* argument_declaration =
      dynamic_cast<const DeclarationExpressionListNode*>(argument->getDeclarationList());
    
    const std::string& variable_id =
      (dynamic_cast<const Variable*>(argument_declaration->getVariable()))->getId();
    argument_names.push_back(variable_id);
    const ArgumentListNode* next_argument =
      dynamic_cast<const ArgumentListNode*>(argument_list_node->getNextArgument());
    extractArgumentNames(next_argument, argument_names);
  }
}

std::vector<std::string> CompilerUtil::getArgumentNamesFromFunctionDeclaration(
  const ArgumentListNode* argument_list_node) {
  std::vector<std::string> argument_names;
  extractArgumentNames(argument_list_node, argument_names);
  for(int i = argument_names.size(); i < 4; i++) {
    argument_names.push_back(NO_ARGUMENT);
  }
  return argument_names;
}

int CompilerUtil::evaluateConstantExpression(const Node* expr) {
  // Base cases.
  if (expr->getType() == "IntegerConstant") {
    const IntegerConstant* integer_constant = dynamic_cast<const IntegerConstant*>(expr);
    return integer_constant->getValue();
  }

  // Recursive cases.
  else if (expr->getType() == "UnaryExpression") {
    const UnaryExpression* unary_expression = dynamic_cast<const UnaryExpression*>(expr);
    int tmp = evaluateConstantExpression(unary_expression->getUnaryExpression());

    // Unary minus operator.
    if (unary_expression->getUnaryType() == "-"){
      return -tmp;
    }
    // Unary not operator.
    else if (unary_expression->getUnaryType() == "~"){
      return ~tmp;
    }
    // Logical not operator.
    else if (unary_expression->getUnaryType() == "!"){
      return !tmp;
    }
  }

  else if (expr->getType() == "MultiplicativeExpression") {
    const MultiplicativeExpression* multiplicative_expression =
      dynamic_cast<const MultiplicativeExpression*>(expr);

    int lhs = evaluateConstantExpression(multiplicative_expression->getLhs());
    int rhs = evaluateConstantExpression(multiplicative_expression->getRhs());
    
    // Multiplication.
    if (multiplicative_expression->getMultiplicativeType() == "*"){
      return lhs * rhs;
    }
    // Division.
    else if (multiplicative_expression->getMultiplicativeType() == "/"){
      return lhs / rhs;
    }
    // Modulo.
    else if (multiplicative_expression->getMultiplicativeType() == "%"){
      return lhs % rhs;
    }
  }

  else if (expr->getType() == "AdditiveExpression") {
    const AdditiveExpression* additive_expression =
      dynamic_cast<const AdditiveExpression*>(expr);

    int lhs = evaluateConstantExpression(additive_expression->getLhs());
    int rhs = evaluateConstantExpression(additive_expression->getRhs());

    // Addition case.
    if (additive_expression->getAdditiveType() == "+"){
      return lhs + rhs;
    }
    // Subtraction case
    if (additive_expression->getAdditiveType() == "-"){
      return lhs - rhs;
    }
  }
  
  else if (expr->getType() == "ShiftExpression") {
    const ShiftExpression* shift_expression = dynamic_cast<const ShiftExpression*>(expr);
    
    int lhs = evaluateConstantExpression(shift_expression->getLhs());
    int rhs = evaluateConstantExpression(shift_expression->getRhs());
    
    // Left shift (<<).
    if (shift_expression->getShiftType() == "<<"){
      return lhs << rhs;
    }  
    // Right shift (>>).
    if (shift_expression->getShiftType() == ">>"){
      return lhs >> rhs;
    }
  }

  else if (expr->getType() == "RelationalExpression") {
    const RelationalExpression* relational_expression =
      dynamic_cast<const RelationalExpression*>(expr);
    
    int lhs = evaluateConstantExpression(relational_expression->getLhs());
    int rhs = evaluateConstantExpression(relational_expression->getRhs());

    // Less than.
    if (relational_expression->getRelationalType() == "<"){
      return lhs < rhs;
    }  
    // Greater than.
    else if (relational_expression->getRelationalType() == ">"){
      return lhs > rhs;
    }  
    // Less or Equal.
    else if (relational_expression->getRelationalType() == "<="){
      return lhs <= rhs;
    }  
    // Greater or Equal.
    else if (relational_expression->getRelationalType() == ">="){
      return lhs >= rhs;
    }
  }

  else if (expr->getType() == "EqualityExpression") {
    const EqualityExpression* equality_expression =
      dynamic_cast<const EqualityExpression*>(expr);
    
    int lhs = evaluateConstantExpression(equality_expression->getLhs());
    int rhs = evaluateConstantExpression(equality_expression->getRhs());

    // Equal to (==).
    if (equality_expression->getEqualityType() == "=="){
      return lhs == rhs;
    }  
    // Not equal to (!=).
    if (equality_expression->getEqualityType() == "!="){
      return lhs != rhs;
    }
  }

  else if (expr->getType() == "AndExpression") {
    const AndExpression* and_expression = dynamic_cast<const AndExpression*>(expr);

    int lhs = evaluateConstantExpression(and_expression->getLhs());
    int rhs = evaluateConstantExpression(and_expression->getRhs());
    return lhs & rhs;  
  }

  else if (expr->getType() == "ExclusiveOrExpression") {
    const ExclusiveOrExpression* exclusive_or_expression =
      dynamic_cast<const ExclusiveOrExpression*>(expr);

    int lhs = evaluateConstantExpression(exclusive_or_expression->getLhs());
    int rhs = evaluateConstantExpression(exclusive_or_expression->getRhs());

    return lhs ^ rhs;
  }

  else if (expr->getType() == "InclusiveOrExpression") {
    const InclusiveOrExpression* inclusive_or_expression =
      dynamic_cast<const InclusiveOrExpression*>(expr);

    int lhs = evaluateConstantExpression(inclusive_or_expression->getLhs());
    int rhs = evaluateConstantExpression(inclusive_or_expression->getRhs());

    return lhs | rhs;
  }

  else if (expr->getType() == "LogicalAndExpression") {
    const LogicalAndExpression* logical_and_expression =
      dynamic_cast<const LogicalAndExpression*>(expr);

    int lhs = evaluateConstantExpression(logical_and_expression->getLhs());
    int rhs = evaluateConstantExpression(logical_and_expression->getRhs());

    return lhs && rhs;
  }

  else if (expr->getType() == "LogicalOrExpression") {
    const LogicalOrExpression* logical_or_expression =
      dynamic_cast<const LogicalOrExpression*>(expr);

    int lhs = evaluateConstantExpression(logical_or_expression->getLhs());
    int rhs = evaluateConstantExpression(logical_or_expression->getRhs());

    return lhs || rhs;
  }

  else if (expr->getType() == "ConditionalExpression") {
    const ConditionalExpression* conditional_expression =
      dynamic_cast<const ConditionalExpression*>(expr);

    int cond = evaluateConstantExpression(conditional_expression->getCondition());
    int expr1 = evaluateConstantExpression(conditional_expression->getExpression1());
    int expr2 = evaluateConstantExpression(conditional_expression->getExpression2());

    return cond ? expr1 : expr2;
  }

  // Unknown or unexpected node.
  else {
    if (Util::DEBUG) {
      std::cerr << "Unkown or unexpected node type while evaluating a constant "
                << "expression: " << expr->getType() << std::endl;
    }
    Util::abort();
  }
}

// RegisterAllocator.

RegisterAllocator::RegisterAllocator() {
  tmp_reg_used_ = std::vector<bool>(tmp_reg_size_, false);
}

std::string RegisterAllocator::requestFreeRegister() {
  for (int i = 0; i < tmp_reg_size_; i++) {
    if (!tmp_reg_used_[i]) {
      tmp_reg_used_[i] = true;
      return "$t" + std::to_string(i); 
    }
  }
  if (Util::DEBUG) {
    std::cerr << "No more temporary registers available." << std::endl;
  }
  Util::abort();
}

void RegisterAllocator::freeRegister(const std::string& reg) {
  int reg_id = std::stoi(reg.substr(2,1));
  if (reg_id < 0 || reg_id > tmp_reg_size_) {
    if (Util::DEBUG) {
      std::cerr << "Trying to free an invalid register: " << reg << std::endl;
    }
    Util::abort();
  }

  if(!tmp_reg_used_[reg_id]) {
    if (Util::DEBUG) {
      std::cerr << "Trying to free an unused register: " << reg << std::endl;
    }
    Util::abort();
  }

  tmp_reg_used_[reg_id] = false;
}

std::vector<std::string> RegisterAllocator::getTemporaryRegistersInUse() {
  std::vector<std::string> used_registers;
  for (int i = 0; i < tmp_reg_size_; i++) {
    if (tmp_reg_used_[i]) {
      used_registers.push_back("$t" + std::to_string(i)); 
    }
  }
  return used_registers;
}

// FunctionContext.

FunctionContext::FunctionContext(int frame_size, const std::string& function_epilogue_label)
  : frame_size_(frame_size), function_epilogue_label_(function_epilogue_label) {}

const std::string& FunctionContext::getFunctionEpilogueLabel() const {
  return function_epilogue_label_;
}

int FunctionContext::placeVariableInStack(const std::string& var_name, 
                                          const std::string& scope_id,
                                          const bool& is_declaration) {
  // A Declaration.
  if (is_declaration) {
    std::pair<std::string, std::string> var_scope(var_name, scope_id);
    if (variable_to_offset_in_stack_frame_.find(var_scope) !=
        variable_to_offset_in_stack_frame_.end()) {
      if (Util::DEBUG) {
        std::cerr << "WARNING: requesting to place in stack a variable already placed in "
                  << "stack: " << var_name << ". Probably overriding its value."
                  << std::endl;
      }
      return variable_to_offset_in_stack_frame_[var_scope];
    }

    for (int i = call_arguments_size_; i < frame_size_ - 2 * word_length_;
         i += word_length_) {
      // Check if the current place is already used (already placed in the map).
      if (offset_in_stack_frame_to_variable_.find(i) ==
          offset_in_stack_frame_to_variable_.end()) {
        // Free place.
        variable_to_offset_in_stack_frame_.insert(
          std::pair<std::pair<std::string, std::string>, int>(var_scope, i));
        offset_in_stack_frame_to_variable_.insert(
          std::pair<int, std::pair<std::string, std::string>>(i, var_scope));

        return i;
      }
    }
    if (Util::DEBUG) {
      std::cerr << "Unable to place variable in stack: " << var_name << std::endl;
    }
    Util::abort();
  }
  // An Assignment.
  else {
    for (int i = scopes_list_.size() - 1; i >= 0; i--){
      std::pair<std::string, std::string> var_scope(var_name, scopes_list_[i]);

      if (variable_to_offset_in_stack_frame_.find(var_scope) !=
          variable_to_offset_in_stack_frame_.end()) {
        return variable_to_offset_in_stack_frame_[var_scope];
      }
    }
    if (Util::DEBUG) {
      std::cerr << "Unable to find variable in stack: " << var_name << std::endl;
    }
    Util::abort();
  }
}

int FunctionContext::getOffsetForVariable(const std::string& var_name) {
  for(int i = scopes_list_.size() - 1; i >= 0; i--){
    std::pair<std::string, std::string> var_scope = {var_name, scopes_list_[i]};
    if (variable_to_offset_in_stack_frame_.find(var_scope) !=
        variable_to_offset_in_stack_frame_.end()) {
      // Existent variable.
      return variable_to_offset_in_stack_frame_[var_scope];
    }
  }
  if (Util::DEBUG) {
    std::cerr << "Variable " << var_name << " has no associated offset in stack frame."
              << std::endl;
  }
  Util::abort();
}

void FunctionContext::saveOffsetForArgument(const std::string& arg_name, int offset,
                                            const std::string& scope_id) {
  if (offset < frame_size_) {
    if (Util::DEBUG) {
      std::cerr << "Offset for argument must be bigger than frame_size for the current "
                << "stack frame, but it is not:" << std::endl
                << "Offset:     " << offset << std::endl
                << "Frame size: " << frame_size_ << std::endl;
    }
    Util::abort();
  }
  std::pair<std::string, std::string> arg_scope = {arg_name, scope_id};
  variable_to_offset_in_stack_frame_.insert(
    std::pair<std::pair<std::string, std::string>, int>(arg_scope, offset));
  offset_in_stack_frame_to_variable_.insert(
    std::pair<int, std::pair<std::string, std::string>>(offset, arg_scope));
}

void FunctionContext::reserveSpaceForArray(const std::string& array_name, int size, 
                                           const std::string& scope_id) {
  // If array name is already in stack, throw error.
  if (variable_to_offset_in_stack_frame_.find(
        std::pair<std::string, std::string>{array_name, scope_id}) !=
      variable_to_offset_in_stack_frame_.end()) {
    if (Util::DEBUG) {
      std::cerr << "Array name already reserved in this scope: " << array_name 
                << "." << std::endl;
    }
    Util::abort();
  }

  // Look for a free space to reserve for the array.
  int start_index = -1;
  for (int i = call_arguments_size_; i < frame_size_ - 2 * word_length_;
        i += word_length_) {
    // Check if the current place is already used (already placed in the map).
    if (offset_in_stack_frame_to_variable_.find(i) ==
        offset_in_stack_frame_to_variable_.end()) {
      // Free place.
      start_index = i;
      break;
    }
  }

  // Place the array in memory.
  int position = 0;
  for (int i = start_index; position < size; i += word_length_) {
    if (i >= frame_size_ - 2 * word_length_) {
      if (Util::DEBUG) {
        std::cerr << "Not enough space in stack frame to allocate array: " << array_name
                  << "." << std::endl;
      }
      Util::abort();
    }

    if (offset_in_stack_frame_to_variable_.find(i) !=
        offset_in_stack_frame_to_variable_.end()) {
      if (Util::DEBUG) {
        std::cerr << "Found memory reserved on the stack where it was supposed to be "
                  << "free, while reserving space for array: " << i << "." << std::endl;
      }
      Util::abort();
    }
    
    std::pair<std::string, std::string> array_scope =
      {array_name + "@" + std::to_string(position), scope_id};
    variable_to_offset_in_stack_frame_.insert(
      std::pair<std::pair<std::string, std::string>, int>(array_scope, i));
    offset_in_stack_frame_to_variable_.insert(
      std::pair<int, std::pair<std::string, std::string>>(i, array_scope));
    position++;
  }

  // Add a placeholder for the name of the array, since it would otherwhise only reachable
  // if you know it is an array.
  // E.g. say you need to check wheter a variable named 'var' is in memory, if it is an
  // array, there is no entry for (var, some_scope), but only for (var@0, some_scope).
  variable_to_offset_in_stack_frame_.insert(
    std::pair<std::pair<std::string, std::string>, int>
    {{array_name, scope_id}, start_index});
}

int FunctionContext::getBaseOffsetForArray(const std::string& array_name) {
  for(int i = scopes_list_.size() - 1; i >= 0; i--){
    std::string array_base_name = array_name + "@0";
    std::pair<std::string, std::string> array_scope = {array_base_name, scopes_list_[i]};
    if (variable_to_offset_in_stack_frame_.find(array_scope) !=
        variable_to_offset_in_stack_frame_.end()) {
      // Existent array.
      return variable_to_offset_in_stack_frame_[array_scope];
    }
  }
  if (Util::DEBUG) {
    std::cerr << "Array " << array_name << " has no associated base offset in stack "
              << "frame." << std::endl;
  }
  Util::abort();
}

const std::string& FunctionContext::getBreakLabel() const {
  if (break_labels_.empty()){
    if(Util::DEBUG){
      std::cerr << "Requesting a break label when not inside a loop nor a switch " 
                << "statement." << std::endl;
    }
    Util::abort();
  }
  return break_labels_.top();
}

const std::string& FunctionContext::getContinueLabel() const {
  if (continue_labels_.empty()){
    if(Util::DEBUG){
      std::cerr << "Requesting a continue label when not inside a loop." << std::endl;
    }
    Util::abort();
  }
  return continue_labels_.top();
}

const std::string& FunctionContext::getDefaultLabel() const {
  if (default_labels_.empty()){
    if(Util::DEBUG){
      std::cerr << "Requesting a default label when not inside a switch statement."
                << std::endl;
    }
    Util::abort();
  }
  return default_labels_.top();
}

void FunctionContext::insertWhileLabels(const std::string& continue_label, 
                                        const std::string& break_label){
  continue_labels_.push(continue_label);              
  break_labels_.push(break_label);              
}

void FunctionContext::removeWhileLabels(){
  continue_labels_.pop();
  break_labels_.pop();
}

void FunctionContext::insertForLabels(const std::string& continue_label,
                                      const std::string& break_label) {
  continue_labels_.push(continue_label);              
  break_labels_.push(break_label);              
}

void FunctionContext::removeForLabels() {
  continue_labels_.pop();
  break_labels_.pop();
}

void FunctionContext::insertSwitchLabels(const std::string& default_label, 
                                         const std::string& break_label){
  default_labels_.push(default_label);
  break_labels_.push(break_label);
}

void FunctionContext::removeSwitchLabels(){
  default_labels_.pop();
  break_labels_.pop();
}

void FunctionContext::insertScope(const std::string& scope_id){
  scopes_list_.push_back(scope_id);
}

void FunctionContext::removeScope(){
  scopes_list_.pop_back();
}

bool FunctionContext::isLocalVariable(const std::string& id) {
  for (std::string scope : scopes_list_) {
    std::pair<std::string, std::string> key(id, scope);
    if (variable_to_offset_in_stack_frame_.find(key) !=
        variable_to_offset_in_stack_frame_.end()) {
      return true;
    }
  }
  return false;
}

// GlobalVariables.

void GlobalVariables::addNewGlobalVariable(const std::string& id,
                                           const std::string& info) {
  if (id_to_info_.find(id) != id_to_info_.end()) {
    if (Util::DEBUG) {
      std::cerr << "Redeclaration of global variable: " << id << "." << std::endl;
    }
    Util::abort();
  }

  id_to_info_.insert(std::pair<std::string, std::string>(id, info));
}

bool GlobalVariables::isGlobalVariable(const std::string& id) const {
  return id_to_info_.find(id) != id_to_info_.end();
}

const std::string& GlobalVariables::getInfoForVariable(const std::string& id) const {
  if (!isGlobalVariable(id)) {
    if (Util::DEBUG) {
      std::cerr << "Id " << id << " does not match any global variable declaration."
                << std::endl;
    }
    Util::abort();
  }

  return id_to_info_.at(id);
}

const std::vector<std::string> GlobalVariables::getAllGlobalVariableIds() const {
  std::vector<std::string> ids;
  for (std::pair<std::string, std::string> id_to_info : id_to_info_) {
    ids.push_back(id_to_info.first);
  }
  return ids;
}

// FunctionDeclarations.

bool FunctionDeclarations::isIdOfDeclaredOnlyFunction(const std::string& id) {
  return ids_.find(id) != ids_.end();
}

void FunctionDeclarations::insertId(const std::string& id) {
  ids_.insert(id);
}
