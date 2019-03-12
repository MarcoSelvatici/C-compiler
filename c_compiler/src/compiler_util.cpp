#include "../inc/compiler_util.hpp"

// CompilerUtil.

unsigned int unique_id_counter = 0;

const std::string CompilerUtil::NO_ARGUMENT = "?";

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

  else if(node_type == "DeclarationExpression") {
    const DeclarationExpression* declaration_expression =
      dynamic_cast<const DeclarationExpression*>(ast_node);
    const std::string& type = declaration_expression->getTypeSpecifier();
    if (type == "int") {
      // Int is 4 bytes.
      return 4;
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
    const DeclarationExpression* argument =
      dynamic_cast<const DeclarationExpression*>(argument_list_node->getArgument());
    const std::string& variable_id =
      (dynamic_cast<const Variable*>(argument->getVariable()))->getId();
    argument_names.push_back(variable_id);
  }
  else if (argument_list_node->hasNextArgument()) {
    // More arguments.
    const DeclarationExpression* argument =
      dynamic_cast<const DeclarationExpression*>(argument_list_node->getArgument());
    const std::string& variable_id =
      (dynamic_cast<const Variable*>(argument->getVariable()))->getId();
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

std::vector<std::string> RegisterAllocator::get_temporary_registers_in_use() {
  std::vector<std::string> used_registers;
  for (int i = 0; i < tmp_reg_size_; i++) {
    if (tmp_reg_used_[i]) {
      used_registers.push_back("$t" + std::to_string(i)); 
    }
  }
  return used_registers;
}

// FunctionContext.

FunctionContext::FunctionContext(int frame_size, const std::string& epilogue_label)
  : frame_size_(frame_size), epilogue_label_(epilogue_label) {}

const std::string& FunctionContext::getEpilogueLabel() const {
  return epilogue_label_;
}

int FunctionContext::placeVariableInStack(const std::string& var_name) {
  // If variable is already in stack, return its current position.
  if (variable_to_offset_in_stack_frame_.find(var_name) !=
      variable_to_offset_in_stack_frame_.end()) {
    if (Util::DEBUG) {
      std::cerr << "WARNING: requesting to place in stack a variable already placed in "
                << "stack: " << var_name << ". Probably overriding its value."
                << std::endl;
    }
    return variable_to_offset_in_stack_frame_[var_name];
  }

  for (int i = call_arguments_size_; i < frame_size_ - 2 * word_length_;
        i += word_length_) {
    // Check if the current place is already used (already placed in the map).
    if (offset_in_stack_frame_to_variable_.find(i) ==
        offset_in_stack_frame_to_variable_.end()) {
      // Free place.
      variable_to_offset_in_stack_frame_.insert(
        std::pair<std::string, int>(var_name, i));
      offset_in_stack_frame_to_variable_.insert(
        std::pair<int, std::string>(i, var_name));
      return i;
    }
  }

  if (Util::DEBUG) {
    std::cerr << "Unable to place variable in stack: " << var_name << std::endl;
  }
  Util::abort();
}

int FunctionContext::getOffsetForVariable(const std::string& var_name) {
  if (variable_to_offset_in_stack_frame_.find(var_name) ==
      variable_to_offset_in_stack_frame_.end()) {
    // Not existent variable.
    if (Util::DEBUG) {
      std::cerr << "Variable " << var_name << " has no associated offset in stack frame."
                << std::endl;
    }
    Util::abort();
  }

  return variable_to_offset_in_stack_frame_[var_name];
}

void FunctionContext::saveOffsetForArgument(const std::string& arg_name, int offset) {
  if (offset < frame_size_) {
    if (Util::DEBUG) {
      std::cerr << "Offset for argument must be bigger than frame_size for the current "
                << "stack frame, but it is not:" << std::endl
                << "Offset:     " << offset << std::endl
                << "Frame size: " << frame_size_ << std::endl;
    }
    Util::abort();
  }
  variable_to_offset_in_stack_frame_.insert(
    std::pair<std::string, int>(arg_name, offset));
  offset_in_stack_frame_to_variable_.insert(
    std::pair<int, std::string>(offset, arg_name));
}

const std::string& FunctionContext::getStartLoopLabel() const {
  if (loop_labels_.size() == 0){
     if (Util::DEBUG) {
      std::cerr << "Currently inside no loops" << std::endl;
    }
    Util::abort();
  }
  return loop_labels_.at(loop_labels_.size() - 2);
}

const std::string& FunctionContext::getDefaultLabel() const {
  if (loop_labels_.size() == 0){
     if (Util::DEBUG) {
      std::cerr << "Currently inside no loops" << std::endl;
    }
    Util::abort();
  }
  return loop_labels_.at(loop_labels_.size() - 2);
}

const std::string& FunctionContext::getEndLoopLabel() const {
  if (loop_labels_.size() == 0){
     if (Util::DEBUG) {
      std::cerr << "Currently inside no loops" << std::endl;
    }
    Util::abort();
  }
  return loop_labels_.at(loop_labels_.size() - 1);
}

void FunctionContext::saveLoopLabels(const std::string& start_loop_label, 
                                     const std::string& end_loop_label) {

  loop_labels_.push_back(start_loop_label);
  loop_labels_.push_back(end_loop_label);
}

void FunctionContext::removeLoopLabels(){
  loop_labels_.pop_back();
  loop_labels_.pop_back();
}

// GlobalVariables.

void GlobalVariables::addNewGlobalVariable(const std::string& id,
                                           const std::string& info) {
    if (id_to_info_.find(id) == id_to_info_.end()) {
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
