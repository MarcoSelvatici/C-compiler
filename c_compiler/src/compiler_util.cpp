#include "../inc/compiler_util.hpp"

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
