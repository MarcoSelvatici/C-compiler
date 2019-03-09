#include "../inc/compiler_util.hpp"

// Compiler util.

unsigned int unique_id_counter = 0;

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

// FunctionContext.

FunctionContext::FunctionContext(int frame_size) : frame_size_(frame_size) {}

int FunctionContext::placeVariableInStack(const std::string& var_name) {
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
