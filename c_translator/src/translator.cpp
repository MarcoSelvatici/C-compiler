#include "../inc/translator.hpp"

#include "../../common/inc/ast.hpp"
#include "../../common/inc/util.hpp"

#include <fstream>

// Indentaion step for the translated code.
const std::string IS = "  ";

// il is indentation level.
void indent(std::ofstream& py_out, int il) {
  for (int i = 0; i < il; i++) {
    py_out << IS;
  }
}

// Inline translation of an arithmetic expression.
// An arithmetic expression could be:
// - integer constant
// - variable
// - unary operation (e.g. unary negation: -4)
// - sum, subtraction
// - multiplication
// - ( arithmetic expression )
void translateArithmeticExpression(std::ofstream& py_out,
                                   const Node* arithmetic_expression) {
  if (Util::DEBUG) {
    std::cerr << "==> Translating arithmetic expression." << std::endl;
  }

  // Base cases.
  if (arithmetic_expression->getType() == "IntegerConstant") {
    const IntegerConstant* integer_constant =
      dynamic_cast<const IntegerConstant*>(arithmetic_expression);
    py_out << integer_constant->getValue();
  }
  else if (arithmetic_expression->getType() == "Variable") {
    const Variable* variable = dynamic_cast<const Variable*>(arithmetic_expression);
    py_out << variable->getId();
  }
  // Recursive cases.
  else if (arithmetic_expression->getType() == "UnaryExpression") {
    const UnaryExpression* unary_expression =
      dynamic_cast<const UnaryExpression*>(arithmetic_expression);
    const std::string& unary_operator = unary_expression->getUnaryType();
    if (unary_operator == "++" || unary_operator == "--" || unary_operator == "*") {
      if (Util::DEBUG) {
        std::cerr << "Invalid unary operator for Python: " << unary_operator << "."
                  << std::endl;
      }
      Util::abort();
    }
    py_out << "(" << unary_operator;
    translateArithmeticExpression(py_out, unary_expression->getUnaryExpression());
    py_out << ")";
  }
  else if (arithmetic_expression->getType() == "AdditiveExpression") {
    const AdditiveExpression* additive_expression =
      dynamic_cast<const AdditiveExpression*>(arithmetic_expression);
    py_out << "(";
    translateArithmeticExpression(py_out, additive_expression->getLhs());
    py_out << " " << additive_expression->getAdditiveType() << " ";
    translateArithmeticExpression(py_out, additive_expression->getRhs());
    py_out << ")";
  }
  else if (arithmetic_expression->getType() == "MultiplicativeExpression") {
    const MultiplicativeExpression* multiplicative_expression =
      dynamic_cast<const MultiplicativeExpression*>(arithmetic_expression);
    py_out << "(";
    translateArithmeticExpression(py_out, multiplicative_expression->getLhs());
    py_out << " " << multiplicative_expression->getMultiplicativeType() << " ";
    translateArithmeticExpression(py_out, multiplicative_expression->getRhs());
    py_out << ")";
  }
  // Unkonwn or unexpected node.
  else {
    if (Util::DEBUG) {
      std::cerr << "Unkown or unexpected node type: " << arithmetic_expression->getType()
                << std::endl;
    }
    Util::abort();
  }
}

void translateVariableDeclaration(std::ofstream& py_out,
                                  const DeclarationExpression* declaration_expression,
                                  int il) {
  if (Util::DEBUG) {
    std::cerr << "==> Translating variable declaration." << std::endl;
  }

  // Only supported type is int.
  if (declaration_expression->getTypeSpecifier() != "int") {
    if (Util::DEBUG) {
      std::cerr << "Unexpected variable with non-int type: "
                << declaration_expression->getTypeSpecifier() << "." << std::endl;
    }
    Util::abort();
  }

  // Extract id.
  const Variable* variable =
      dynamic_cast<const Variable*>(declaration_expression->getVariable());
  const std::string& variable_id = variable->getId();
  // Evaluate rhs.
  if (declaration_expression->hasRhs()) {
    // TODO: implement arithmetic expressions.
    indent(py_out, il);
    py_out << variable_id << " = ";
    // Do not add any indentation, since it is inline.
    translateArithmeticExpression(py_out, declaration_expression->getRhs());
    py_out << std::endl;
  } else {
    indent(py_out, il);
    py_out << variable_id << " = 0" << std::endl;
  }
}

void translateStatement(std::ofstream& py_out, const Node* statement, int il) {
  if (Util::DEBUG) {
    std::cerr << "==> Translating statement." << std::endl;
  }

  indent(py_out, il);
  py_out << statement->getType();
  py_out << std::endl;
}

// 2 possible scenarios for each node(statement, next_statement):
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
    const DeclarationExpression* argument =
      dynamic_cast<const DeclarationExpression*>(argument_list_node->getArgument());
    
    // Only supported type is int.
    if(argument->getTypeSpecifier() != "int") {
      if (Util::DEBUG) {
        std::cerr << "Unexpected argument with non-int type: "
                  << argument->getTypeSpecifier() << "." << std::endl;
      }
      Util::abort();
    }

    // Extract id.
    const std::string& id =
        (dynamic_cast<const Variable*>(argument->getVariable()))->getId();
    py_out << id;
  }
  // Recursive case.
  else if (argument_list_node->hasNextArgument()) {
    // Argument exists and has successor.
    const DeclarationExpression* argument =
      dynamic_cast<const DeclarationExpression*>(argument_list_node->getArgument());
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

    // Extract id for the current argument.
    const std::string& id =
        (dynamic_cast<const Variable*>(argument->getVariable()))->getId();
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

  // Translate function body.
  const StatementListNode* statement_list_node =
    dynamic_cast<const StatementListNode*>(function_definition->getBody());
  translateStatementList(py_out, statement_list_node, 1);
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
  if (ast->getType() == "DeclarationExpression") {
    const DeclarationExpression* declaration_expression =
      dynamic_cast<const DeclarationExpression*>(ast);
    translateVariableDeclaration(py_out, declaration_expression, 0);
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

void translateAST(std::vector<const Node*> ast_roots, std::ofstream& py_out ) {
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
