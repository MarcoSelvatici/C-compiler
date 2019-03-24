#include "../inc/translator.hpp"

#include "../../common/inc/ast.hpp"
#include "../../common/inc/util.hpp"

#include <fstream>
#include <unordered_set>

// Indentaion step for the translated code.
const std::string IS = "  ";

// il is indentation level.
void indent(std::ofstream& py_out, int il) {
  for (int i = 0; i < il; i++) {
    py_out << IS;
  }
}

// Global variables hack.
// Consists of two parts.
// Part 1: every time a global variable is encountered, it is entered in a set that keeps
//         track of its declaration.
// Part 2: every time a function is being defined, go through its AST and check the
//         variables that are used. If one of those variables is from the global scope,
//         add the 'global variable_name' statement. [not implemented yet]
// Part 2: add all global variables at the beginning of each function. [unideal
//         implemented solution]
// We are not dealing with variable shadowing.

std::unordered_set<std::string> global_variables;

void addVariableToGlobals(const std::string& variable_name) {
  global_variables.insert(variable_name);
}

bool isGlobal(const std::string& variable_name) {
  return global_variables.find(variable_name) != global_variables.end();
}

void addGlobalStatements(std::ofstream& py_out) {
  for (auto global_variable_id : global_variables) {
    py_out << IS << "global " << global_variable_id << std::endl;
  }
}

// Function declarations to allow mutually recursive functions.
void translateStatement(std::ofstream& py_out, const Node* statement, int il);
void translateStatementList(std::ofstream& py_out,
                            const StatementListNode* statement_list_node, int il);
void translateFunctionCall(std::ofstream& py_out, const FunctionCall* function_call);
void translateFunctionCallParametersList(std::ofstream& py_out,
                                         const ParametersListNode* parameters_list_node);
void translateAssignmentExpression(std::ofstream& py_out,
                                   const AssignmentExpression* assignment_expression);

// Inline translation of an arithmetic or logical expression.
// An arithmetic or logical expression could be:
// - integer constant
// - variable
// - unary operation (e.g. unary negation: -4)
// - sum, subtraction
// - multiplication
// - relational expression (i.e. >, >=, <, <=)
// - equality expression (i.e. ==, !=)
// - logical and and or (i.e. &&, ||) 
// - function call
// - ( arithmetic expression ) --> no need for an if because it is implicitly built in the
//                                 structure of AST.
void translateArithmeticOrLogicalExpression(
  std::ofstream& py_out, const Node* arithmetic_or_logical_expression) {
  if (Util::DEBUG) {
    std::cerr << "==> Translating arithmetic expression." << std::endl;
  }

  // Base cases.
  if (arithmetic_or_logical_expression->getType() == "IntegerConstant") {
    const IntegerConstant* integer_constant =
      dynamic_cast<const IntegerConstant*>(arithmetic_or_logical_expression);
    py_out << integer_constant->getValue();
  }
  else if (arithmetic_or_logical_expression->getType() == "Variable") {
    const Variable* variable =
      dynamic_cast<const Variable*>(arithmetic_or_logical_expression);
    py_out << variable->getId();
  }
  // Recursive cases.
  else if (arithmetic_or_logical_expression->getType() == "UnaryExpression") {
    const UnaryExpression* unary_expression =
      dynamic_cast<const UnaryExpression*>(arithmetic_or_logical_expression);
    const std::string& unary_operator = unary_expression->getUnaryType();
    if (unary_operator == "++" || unary_operator == "--" || unary_operator == "*") {
      if (Util::DEBUG) {
        std::cerr << "Invalid unary operator for Python: " << unary_operator << "."
                  << std::endl;
      }
      Util::abort();
    }
    py_out << "(" << unary_operator;
    translateArithmeticOrLogicalExpression(py_out,
                                           unary_expression->getUnaryExpression());
    py_out << ")";
  }
  else if (arithmetic_or_logical_expression->getType() == "AdditiveExpression") {
    const AdditiveExpression* additive_expression =
      dynamic_cast<const AdditiveExpression*>(arithmetic_or_logical_expression);
    py_out << "(";
    translateArithmeticOrLogicalExpression(py_out, additive_expression->getLhs());
    py_out << " " << additive_expression->getAdditiveType() << " ";
    translateArithmeticOrLogicalExpression(py_out, additive_expression->getRhs());
    py_out << ")";
  }
  else if (arithmetic_or_logical_expression->getType() == "MultiplicativeExpression") {
    const MultiplicativeExpression* multiplicative_expression =
      dynamic_cast<const MultiplicativeExpression*>(arithmetic_or_logical_expression);
    py_out << "(";
    translateArithmeticOrLogicalExpression(py_out, multiplicative_expression->getLhs());
    py_out << " " << multiplicative_expression->getMultiplicativeType() << " ";
    translateArithmeticOrLogicalExpression(py_out, multiplicative_expression->getRhs());
    py_out << ")";
  }
  else if (arithmetic_or_logical_expression->getType() == "EqualityExpression") {
    const EqualityExpression* equality_expression =
      dynamic_cast<const EqualityExpression*>(arithmetic_or_logical_expression);
    py_out << "(";
    translateArithmeticOrLogicalExpression(py_out, equality_expression->getLhs());
    py_out << " " << equality_expression->getEqualityType() << " ";
    translateArithmeticOrLogicalExpression(py_out, equality_expression->getRhs());
    py_out << ")";
  }
  else if (arithmetic_or_logical_expression->getType() == "RelationalExpression") {
    const RelationalExpression* relational_expression =
      dynamic_cast<const RelationalExpression*>(arithmetic_or_logical_expression);
    py_out << "(";
    translateArithmeticOrLogicalExpression(py_out, relational_expression->getLhs());
    py_out << " " << relational_expression->getRelationalType() << " ";
    translateArithmeticOrLogicalExpression(py_out, relational_expression->getRhs());
    py_out << ")";
  }
  else if (arithmetic_or_logical_expression->getType() == "LogicalOrExpression") {
    const LogicalOrExpression* logical_or_expression =
      dynamic_cast<const LogicalOrExpression*>(arithmetic_or_logical_expression);
    py_out << "(";
    translateArithmeticOrLogicalExpression(py_out, logical_or_expression->getLhs());
    py_out << " or ";
    translateArithmeticOrLogicalExpression(py_out, logical_or_expression->getRhs());
    py_out << ")";
  }
  else if (arithmetic_or_logical_expression->getType() == "LogicalAndExpression") {
    const LogicalAndExpression* logical_and_expression =
      dynamic_cast<const LogicalAndExpression*>(arithmetic_or_logical_expression);
    py_out << "(";
    translateArithmeticOrLogicalExpression(py_out, logical_and_expression->getLhs());
    py_out << " and ";
    translateArithmeticOrLogicalExpression(py_out, logical_and_expression->getRhs());
    py_out << ")";
  }
  else if (arithmetic_or_logical_expression->getType() == "FunctionCall") {
    const FunctionCall* function_call =
      dynamic_cast<const FunctionCall*>(arithmetic_or_logical_expression);
    translateFunctionCall(py_out, function_call);
  }
  else if (arithmetic_or_logical_expression->getType() == "AssignmentExpression") {
    const AssignmentExpression* assignment_expression =
      dynamic_cast<const AssignmentExpression*>(arithmetic_or_logical_expression);
    translateAssignmentExpression(py_out, assignment_expression);
  }
  // Unkonwn or unexpected node.
  else {
    if (Util::DEBUG) {
      std::cerr << "Unkown or unexpected node type: "
                << arithmetic_or_logical_expression->getType() << std::endl;
    }
    Util::abort();
  }
}

// Inline translation of a function call.
void translateFunctionCall(std::ofstream& py_out, const FunctionCall* function_call) {
  if (Util::DEBUG) {
    std::cerr << "==> Translating function call." << std::endl;
  }

  const std::string& function_id = function_call->getFunctionId();
  const ParametersListNode* parameters_list_node =
    dynamic_cast<const ParametersListNode*>(function_call->getParametersList());
  py_out << function_id << "(";
  translateFunctionCallParametersList(py_out, parameters_list_node);
  py_out << ")";
}

// 3 possible scenarios for each node(parameter, next_parameter):
// node(nullptr, nullptr)          --> parameter list is empty.
// node(parameter, nullptr)        --> only one parameter left.
// node(parameter, next_parameter) --> parameter exists and has successor.
void translateFunctionCallParametersList(std::ofstream& py_out,
                                         const ParametersListNode* parameters_list_node) {
  if (Util::DEBUG) {
    std::cerr << "==> Translating function call parameters list." << std::endl;
  }

  // Base cases.
  if (parameters_list_node->isEmptyParameterList()) {
    return;
  }
  else if(!parameters_list_node->hasNextParameter()) {
    // Only one parameter left.
    const Node* parameter = parameters_list_node->getParameter();
    translateArithmeticOrLogicalExpression(py_out, parameter);
  }
  // Recursive case.
  else if (parameters_list_node->hasNextParameter()) {
    // Parameter exists and has successor.
    const Node* parameter = parameters_list_node->getParameter();
    const ParametersListNode* next_parameter =
      dynamic_cast<const ParametersListNode*>(parameters_list_node->getNextParameter());
    translateArithmeticOrLogicalExpression(py_out, parameter);
    py_out << ", ";
    translateFunctionCallParametersList(py_out, next_parameter);
  }
}

void translateDeclarationExpressionList(std::ofstream& py_out,
                                        const DeclarationExpressionList* declaration_expression_list,
                                        int il) {
  if (Util::DEBUG) {
    std::cerr << "==> Translating variable declaration." << std::endl;
  }

  // Only supported type is int.
  if (declaration_expression_list->getTypeSpecifier() != "int") {
    if (Util::DEBUG) {
      std::cerr << "Unexpected variable with non-int type: "
                << declaration_expression_list->getTypeSpecifier() << "." << std::endl;
    }
    Util::abort();
  }

  const DeclarationExpressionListNode* declaration_expression_list_node =
    dynamic_cast<const DeclarationExpressionListNode*>
    (declaration_expression_list->getDeclarationList());
  while (declaration_expression_list_node != nullptr){
    // Extract id.
    const Variable* variable =
        dynamic_cast<const Variable*>(declaration_expression_list_node->getVariable());
    const std::string& variable_id = variable->getId();
  
    // Evaluate rhs.
    if (declaration_expression_list_node->hasRhs()) {
      indent(py_out, il);
      py_out << variable_id << " = ";
      // Do not add any indentation, since it is inline.
      translateArithmeticOrLogicalExpression(py_out, 
                                            declaration_expression_list_node->getRhs());
      py_out << std::endl;
    } else {
      indent(py_out, il);
      py_out << variable_id << " = 0";
      py_out << std::endl;
    }

    // If variable is global (indentation level == 0), add it to the set of globals.
    if (il == 0) {
      addVariableToGlobals(variable_id);
    }
    
    if (declaration_expression_list_node->hasNext()){
      declaration_expression_list_node = dynamic_cast<const DeclarationExpressionListNode*>
      (declaration_expression_list_node->getNext());
    } else {
      break;
    }
  }
}

void translateAssignmentExpression(std::ofstream& py_out,
                                   const AssignmentExpression* assignment_expression) {
  if (Util::DEBUG) {
    std::cerr << "==> Translating assignment expression." << std::endl;
  }

  // Extract id.
  const Variable* variable =
      dynamic_cast<const Variable*>(assignment_expression->getVariable());
  const std::string& variable_id = variable->getId();
  // Evaluate rhs.
  py_out << variable_id << " = ";
  // Do not add any indentation, since it is inline.
  translateArithmeticOrLogicalExpression(py_out, assignment_expression->getRhs());
}

void translateReturnStatement(std::ofstream& py_out,
                              const ReturnStatement* return_statement, int il) {
  if (Util::DEBUG) {
    std::cerr << "==> Translating return statement." << std::endl;
  }

  if (return_statement->hasExpression()) {
    indent(py_out, il);
    py_out << "return ";
    translateArithmeticOrLogicalExpression(py_out, return_statement->getExpression());
    py_out << std::endl;
  } else {
    indent(py_out, il);
    py_out << "return";
    py_out << std::endl;
  }
}

void translateIfStatement(std::ofstream& py_out, const IfStatement* if_statement,
                          int il) {
  if (Util::DEBUG) {
    std::cerr << "==> Translating if statement." << std::endl;
  }

  // Translate condition.
  indent(py_out, il);
  py_out << "if ";
  translateArithmeticOrLogicalExpression(py_out, if_statement->getCondition());
  py_out << ":";
  py_out << std::endl;

  // Translate if body.
  // We could have a single statement (no brackets) or a compound statement.
  if (if_statement->getIfBody()->getType() == "StatementListNode") {
    // Compound statement (brackets).
    const StatementListNode* body =
      dynamic_cast<const StatementListNode*>(if_statement->getIfBody());
    if (body->isEmptyStatementList()) {
      if(Util::DEBUG) {
        std::cerr << "Body of if cannot be empty in Python." << std::endl;
      }
      Util::abort();
    }
    translateStatementList(py_out, body, il + 1);
  } else {
    // Single statement (no brackets).
    translateStatement(py_out, if_statement->getIfBody(), il + 1);
  }

  // Translate else body, if present.
  if (if_statement->hasElseBody()) {
    indent(py_out, il);
    py_out << "else:";
    py_out << std::endl;

    if (if_statement->getElseBody()->getType() == "StatementListNode") {
      // Compound statement (brackets).
      const StatementListNode* body =
        dynamic_cast<const StatementListNode*>(if_statement->getElseBody());
      if (body->isEmptyStatementList()) {
        if(Util::DEBUG) {
          std::cerr << "Body of else cannot be empty in Python." << std::endl;
        }
        Util::abort();
      }
      translateStatementList(py_out, body, il + 1);
    } else {
      // Single statement (no brackets).
      translateStatement(py_out, if_statement->getElseBody(), il + 1);
    }
  }
}

void translateWhileStatement(std::ofstream& py_out, const WhileStatement* while_statement,
                             int il) {
  if (Util::DEBUG) {
    std::cerr << "==> Translating while statement." << std::endl;
  }

  // Translate condition.
  indent(py_out, il);
  py_out << "while ";
  translateArithmeticOrLogicalExpression(py_out, while_statement->getCondition());
  py_out << ":";
  py_out << std::endl;

  // Translate while body.
  // We could have a single statement (no brackets) or a compound statement.
  if (while_statement->getBody()->getType() == "StatementListNode") {
    // Compound statement (brackets).
    const StatementListNode* body =
      dynamic_cast<const StatementListNode*>(while_statement->getBody());
    if (body->isEmptyStatementList()) {
      if(Util::DEBUG) {
        std::cerr << "Body of while cannot be empty in Python." << std::endl;
      }
      Util::abort();
    }
    translateStatementList(py_out, body, il + 1);
  } else {
    // Single statement (no brackets).
    translateStatement(py_out, while_statement->getBody(), il + 1);
  }

}

void translateCompoundStatement(std::ofstream& py_out,
                                const CompoundStatement* compound_statement, int il) {
  if (Util::DEBUG) {
    std::cerr << "==> Translating compound statement." << std::endl;
  }

  if (compound_statement->hasStatementList()) {
    const StatementListNode* body =
      dynamic_cast<const StatementListNode*>(compound_statement->getStatementList());
    translateStatementList(py_out, body, il);
  } else {
    indent(py_out, il);
    py_out << "pass" << std::endl;
    // Statement list is empty.
    return;
  }
}

// Supported types of statement:
// - declaration expression
// - assignment expression
// - if else
// - while
// - return
void translateStatement(std::ofstream& py_out, const Node* statement, int il) {
  if (Util::DEBUG) {
    std::cerr << "==> Translating statement." << std::endl;
  }

  const std::string& statement_type = statement->getType();
  
  if (statement_type == "CompoundStatement") {
    const CompoundStatement* compound_statement =
      dynamic_cast<const CompoundStatement*>(statement);
    translateCompoundStatement(py_out, compound_statement, il);
  }
  else if (statement_type == "DeclarationExpressionList") {
    const DeclarationExpressionList* declaration_expression_list =
      dynamic_cast<const DeclarationExpressionList*>(statement);
    translateDeclarationExpressionList(py_out, declaration_expression_list, il);
  }
  else if (statement_type == "IfStatement") {
    const IfStatement* if_statement = dynamic_cast<const IfStatement*>(statement);
    translateIfStatement(py_out, if_statement, il);
  }
  else if (statement_type == "WhileStatement") {
    const WhileStatement* while_statement =
      dynamic_cast<const WhileStatement*>(statement);
    translateWhileStatement(py_out, while_statement, il);
  }
  else if (statement_type == "ReturnStatement") {
    const ReturnStatement* return_statement =
      dynamic_cast<const ReturnStatement*>(statement);
    translateReturnStatement(py_out, return_statement, il);
  }
  else if (statement_type == "EmptyExpression") {
    // Do nothing.
  }
  else if (statement_type == "IntegerConstant" ||
           statement_type == "Variable" ||
           statement_type == "UnaryExpression" ||
           statement_type == "PostfixExpression" ||
           statement_type == "MultiplicativeExpression" ||
           statement_type == "AdditiveExpression" ||
           statement_type == "ShiftExpression" ||
           statement_type == "RelationalExpression" ||
           statement_type == "EqualityExpression" ||
           statement_type == "AndExpression" ||
           statement_type == "ExclusiveOrExpression" ||
           statement_type == "InclusiveOrExpression" ||
           statement_type == "LogicalAndExpression" ||
           statement_type == "LogicalOrExpression" ||
           statement_type == "ConditionalExpression" ||
           statement_type == "FunctionCall" ||
           statement_type == "AssignmentExpression") {
    // Operation that do no return anything, e.g.:
    // a + b;
    indent(py_out, il);
    translateArithmeticOrLogicalExpression(py_out, statement);
    py_out << std::endl;
  }
  // Unkonwn or unexpected node.
  else {
    if (Util::DEBUG) {
      std::cerr << "Unkown or unexpected node type: " << statement->getType()
                << std::endl;
    }
    Util::abort();
  }
}

// 3 possible scenarios for each node(statement, next_statement):
// node(nullptr, nullptr)          --> statement list is empty.
// node(statement, nullptr)        --> only one statement left.
// node(statement, next_statement) --> statement exists and has successor.
void translateStatementList(std::ofstream& py_out,
                            const StatementListNode* statement_list_node, int il) {
  if (Util::DEBUG) {
    std::cerr << "==> Translating statement list." << std::endl;
  }

  // Base cases.
  if (!statement_list_node->hasStatement()) {
    indent(py_out, il);
    py_out << "pass" << std::endl;
    // Statement list is empty.
    return;
  }
  else if(!statement_list_node->hasNextStatement()) {
    // Only one statement left.
    const Node* statement = statement_list_node->getStatement();
    translateStatement(py_out, statement, il);
  }
  // Recursive case.
  else if (statement_list_node->hasNextStatement()) {
    // Statement exists and has successor.
    const Node* statement = statement_list_node->getStatement();
    const StatementListNode* next_statement =
      dynamic_cast<const StatementListNode*>(statement_list_node->getNextStatement());
    translateStatement(py_out, statement, il);
    translateStatementList(py_out, next_statement, il);
  }
}

// 3 possible scenarios for each node(argument, next_argument):
// node(nullptr, nullptr)        --> argument list is empty.
// node(argument, nullptr)       --> only one argument left.
// node(argument, next_argument) --> argument exists and has successor.
void translateFunctionArgumentList(std::ofstream& py_out,
                                   const ArgumentListNode* argument_list_node) {
  if (Util::DEBUG) {
    std::cerr << "==> Translating function arguments list." << std::endl;
  }

  // Base cases.
  if (!argument_list_node->hasArgument()) {
    // Argument list is empty.
    return;
  }
  else if(!argument_list_node->hasNextArgument()) {
    // Only one argument left.
    const DeclarationExpressionList* argument =
      dynamic_cast<const DeclarationExpressionList*>(argument_list_node->getArgument());
    
    // Only supported type is int.
    if(argument->getTypeSpecifier() != "int") {
      if (Util::DEBUG) {
        std::cerr << "Unexpected argument with non-int type: "
                  << argument->getTypeSpecifier() << "." << std::endl;
      }
      Util::abort();
    }

    const DeclarationExpressionListNode* argument_declaration =
      dynamic_cast<const DeclarationExpressionListNode*>(argument->getDeclarationList());
    
    // Extract id.
    const std::string& id =
        (dynamic_cast<const Variable*>(argument_declaration->getVariable()))->getId();
    py_out << id;
  }
  // Recursive case.
  else if (argument_list_node->hasNextArgument()) {
    // Argument exists and has successor.
    const DeclarationExpressionList* argument =
      dynamic_cast<const DeclarationExpressionList*>(argument_list_node->getArgument());
    const ArgumentListNode* next_argument =
      dynamic_cast<const ArgumentListNode*>(argument_list_node->getNextArgument());
    
    // Only supported type is int.
    if(argument->getTypeSpecifier() != "int") {
      if (Util::DEBUG) {
        std::cerr << "Unexpected argument with non-int type: "
                  << argument->getTypeSpecifier() << "." << std::endl;
      }
      Util::abort();
    }

    const DeclarationExpressionListNode* argument_declaration =
      dynamic_cast<const DeclarationExpressionListNode*>(argument->getDeclarationList());
    
    // Extract id for the current argument.
    const std::string& id =
        (dynamic_cast<const Variable*>(argument_declaration->getVariable()))->getId();
    py_out << id << ", ";
    translateFunctionArgumentList(py_out, next_argument);
  }
}

void translateFunctionDefinition(std::ofstream& py_out,
                                 const FunctionDefinition* function_definition) {
  if (Util::DEBUG) {
    std::cerr << "==> Translating function definition." << std::endl;
  }

  // Check type of the function. Only suppeorted int and void.
  if (function_definition->getTypeSpecifier() != "int" &&
      function_definition->getTypeSpecifier() != "void") {
    if (Util::DEBUG) {
      std::cerr << "Unexpected function with non-int and non-void types: "
                << function_definition->getTypeSpecifier() << "." << std::endl;
    }
    Util::abort();
  }

  // Get function ID.
  const std::string& id =
    (dynamic_cast<const Variable*>(function_definition->getName()))->getId();

  // Translate function signature.
  py_out << std::endl;
  py_out << "def " << id << "(";
  const ArgumentListNode* argument_list_node =
    dynamic_cast<const ArgumentListNode*>(function_definition->getArgumentList());
  translateFunctionArgumentList(py_out, argument_list_node);
  py_out << "):";
  py_out << std::endl;

  addGlobalStatements(py_out);

  // Translate function body.
  const CompoundStatement* compound_statement =
    dynamic_cast<const CompoundStatement*>(function_definition->getBody());
  translateCompoundStatement(py_out, compound_statement, 1);
  py_out << std::endl;
}

// Translates root level, i.e. global scope. Includes:
// - definition of global integer variables.
// - definition of functions.
void translateRootLevel(std::ofstream& py_out, const Node* ast) {
  if (Util::DEBUG) {
    std::cerr << "==> Translating root level." << std::endl;
  }

  // Global variable declaration.
  if (ast->getType() == "DeclarationExpressionList") {
    const DeclarationExpressionList* declaration_expression_list =
      dynamic_cast<const DeclarationExpressionList*>(ast);
    translateDeclarationExpressionList(py_out, declaration_expression_list, 0);
  }
  // Function definition.
  else if (ast->getType() == "FunctionDefinition") {
    const FunctionDefinition* function_definition =
      dynamic_cast<const FunctionDefinition*>(ast);
    translateFunctionDefinition(py_out, function_definition);
  }
  // Unkonwn or unexpected node.
  else {
    if (Util::DEBUG) {
      std::cerr << "Unkown or unexpected node type: " << ast->getType() << std::endl;
    }
    Util::abort();
  }
}

void translateAST(const std::vector<const Node*>& ast_roots, std::ofstream& py_out ) {
  for (const Node* ast : ast_roots) {
    if(Util::DEBUG) {
      std::cerr << std::endl << std::endl
                << "============ AST ============" << std::endl;
      ast->print(std::cerr, "");
      std::cerr << std::endl << std::endl
                << "======== TRANSLATION ========" << std::endl;
    }
    translateRootLevel(py_out, ast);
  }

  // Invoke main as the starting point
  py_out << std::endl;
  py_out << "if __name__ == '__main__':" << std::endl
         << IS << "import sys" << std::endl
         << IS << "ret = main()" << std::endl
         << IS << "sys.exit(ret)" << std::endl;
}

int translate(const std::string& source_file_name,
              const std::string& destination_file_name) {
  FILE* file_in;
  if (!(file_in = fopen(source_file_name.c_str(), "r"))) {
    std::cerr << "Cannot open source file: '" << source_file_name << "'." << std::endl;
    return 1;
  }
  // Set file Flex and Yacc will read from.
  yyset_in(file_in);

  // Prepare python output file.
  std::ofstream py_out;
  py_out.open(destination_file_name);

  // Implement translator.
  std::vector<const Node*> ast_roots = parseAST();
  translateAST(ast_roots, py_out);

  // Close the files.
  fclose(file_in);
  py_out.close();
  return 0;
}
