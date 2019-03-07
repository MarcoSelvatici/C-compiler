#include "../inc/compiler.hpp"
#include "../inc/compiler_util.hpp"

#include "../../common/inc/ast.hpp"
#include "../../common/inc/util.hpp"

#include <fstream>

#define WORD_LENGTH 4

// Inline compilation of an arithmetic or logical expression.
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
void compileArithmeticOrLogicalExpression(
  std::ofstream& asm_out, const Node* arithmetic_or_logical_expression,
  const std::string& dest_reg) {
  if (Util::DEBUG) {
    std::cerr << "==> Compiling arithmetic expression." << std::endl;
  }

  // Base cases.
  if (arithmetic_or_logical_expression->getType() == "IntegerConstant") {
    const IntegerConstant* integer_constant =
      dynamic_cast<const IntegerConstant*>(arithmetic_or_logical_expression);
    
    //add immediate constant into destination register
    asm_out <<"addi " <<dest_reg <<" " <<integer_constant->getValue();
  }
  else if (arithmetic_or_logical_expression->getType() == "Variable") {
    const Variable* variable =
      dynamic_cast<const Variable*>(arithmetic_or_logical_expression);
    // TODO (assuming we have a map that maps: variable_name --> its_location_in_memory)
    // retrieve from map variable->getId();
    //asm_out << "lw " <<dest_reg <<"posinmemory" <<std::endl; 
  }

  // Recursive cases.
  else if (arithmetic_or_logical_expression->getType() == "UnaryExpression") {
    const UnaryExpression* unary_expression =
      dynamic_cast<const UnaryExpression*>(arithmetic_or_logical_expression);
    // TODO
    //compileUnaryExpression(asm_out, unary_expression, dest_reg);
  }
  else if (arithmetic_or_logical_expression->getType() == "AdditiveExpression") {
    const AdditiveExpression* additive_expression =
      dynamic_cast<const AdditiveExpression*>(arithmetic_or_logical_expression);

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
  // Unkonwn or unexpected node.
  else {
    if (Util::DEBUG) {
      std::cerr << "Unkown or unexpected node type: "
                << arithmetic_or_logical_expression->getType() << std::endl;
    }
    Util::abort();
  }
}


void compileReturnStatement(std::ofstream& asm_out,
                              const ReturnStatement* return_statement) {
  if (Util::DEBUG) {
    std::cerr << "==> Compiling return statement." << std::endl;
  }

  if (return_statement->hasExpression()) {
    // TODO
    // std::string dest_reg = Context::getFreeRegister();
    // compileArithmeticOrLogicalExpression(asm_out, return_statement->getExpression(), dest_reg);
    
    // move return value in $2.
    asm_out << "move\t $v0 " /*<< dest_reg*/ <<std::endl;
  } 
}

// Supported types of statement:
// - declaration expression
// - assignment expression
// - if else
// - while
// - return
void compileStatement(std::ofstream& asm_out, const Node* statement) {
  if (Util::DEBUG) {
    std::cerr << "==> Compiling statement." << std::endl;
  }

  const std::string& statement_type = statement->getType();
  
  if (statement_type == "ReturnStatement") {
    const ReturnStatement* return_statement =
      dynamic_cast<const ReturnStatement*>(statement);
    //compileReturnStatement(py_out, return_statement);
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
void compileStatementList(std::ofstream& asm_out,
                          const StatementListNode* statement_list_node) {
  if (Util::DEBUG) {
    std::cerr << "==> Compiling statement list." << std::endl;
  }

  // Base cases.
  if (statement_list_node->isEmptyStatementList()) {
    // Statement list is empty.
    return;
  }
  else if(!statement_list_node->hasNextStatement()) {
    // Only one statement left.
    const Node* statement = statement_list_node->getStatement();
    compileStatement(asm_out, statement);
  }
  // Recursive case.
  else if (statement_list_node->hasNextStatement()) {
    // Statement exists and has successor.
    const Node* statement = statement_list_node->getStatement();
    const StatementListNode* next_statement =
      dynamic_cast<const StatementListNode*>(statement_list_node->getNextStatement());
    compileStatement(asm_out, statement);
    compileStatementList(asm_out, next_statement);
  }
}

void compileFunctionDefinition(std::ofstream& asm_out,
                               const FunctionDefinition* function_definition) {
  if (Util::DEBUG) {
    std::cerr << "==> Compiling function definition." << std::endl;
  }

  // Get function components.
  const std::string& type = function_definition->getTypeSpecifier();
  const std::string& id =
    (dynamic_cast<const Variable*>(function_definition->getName()))->getId();
  const ArgumentListNode* argument_list_node =
    dynamic_cast<const ArgumentListNode*>(function_definition->getArgumentList());
  const StatementListNode* statement_list_node =
    dynamic_cast<const StatementListNode*>(function_definition->getBody());

  // Check type of the function. Only supported so far: int and void.
  if (type != "int" && type != "void") {
    if (Util::DEBUG) {
      std::cerr << "Unexpected function with non-int and non-void types: "
                << function_definition->getTypeSpecifier() << "." << std::endl;
    }
    Util::abort();
  }

  int bytes_to_allocate = CompilerUtil::countBytesForDeclarationsInFunction(function_definition);
  // 6 words in each function frame.
  // See: https://minnie.tuhs.org/CompArch/Labs/week4.html section 3.5 
  int frame_size = bytes_to_allocate + 6 * WORD_LENGTH;

  asm_out << "#### Function: " << id << " ####" << std::endl;

  // Function prologue.
  asm_out << "## Prologue ##" << std::endl;
  // Label.
  asm_out << id << ":" << std::endl;
  // Move stack pointer to bottom of the frame.
  asm_out << "addiu\t $sp, $sp, -" << frame_size << std::endl;
  // Save return address at the top of the frame.
  asm_out << "sw\t $ra, " << frame_size - WORD_LENGTH << "($sp)" << std::endl;
  // Save previous frame pointer in the second word from the top of this frame.
  asm_out << "sw\t $fp, " << frame_size - 2 * WORD_LENGTH << "($sp)" << std::endl;
  // Move frame pointer to the end of this frame.
  asm_out << "move\t $fp, $sp" << std::endl;

  // Store the first 4 arguments of the function in the previous function frame.
  // Only if not main.
  if (id != "main") {
    asm_out << "sw\t $a0, " << 0 * WORD_LENGTH + frame_size << "($sp)" << std::endl;
    asm_out << "sw\t $a1, " << 1 * WORD_LENGTH + frame_size << "($sp)" << std::endl;
    asm_out << "sw\t $a2, " << 2 * WORD_LENGTH + frame_size << "($sp)" << std::endl;
    asm_out << "sw\t $a3, " << 3 * WORD_LENGTH + frame_size << "($sp)" << std::endl;
  }

  // Function body.
  asm_out << "## Body ##" << std::endl;
  compileStatementList(asm_out, statement_list_node);

  // Function epilogue.
  asm_out << "## Epilogue ##" << std::endl;
  // Restore the first 4 arguments of the function from the previous function frame.
  // Only if not main.
  if (id != "main") {
    asm_out << "lw\t $a0, " << 0 * WORD_LENGTH + frame_size << "($fp)" << std::endl;
    asm_out << "lw\t $a1, " << 1 * WORD_LENGTH + frame_size << "($fp)" << std::endl;
    asm_out << "lw\t $a2, " << 2 * WORD_LENGTH + frame_size << "($fp)" << std::endl;
    asm_out << "lw\t $a3, " << 3 * WORD_LENGTH + frame_size << "($fp)" << std::endl;
  }

  // Move stack pointer to frame pointer.
  asm_out << "move\t $sp, $fp" << std::endl;
  // Restore the return address.
  asm_out << "lw\t $ra," << frame_size - WORD_LENGTH << "($sp)" << std::endl;
  // Restore the previous frame pointer.
  asm_out << "lw\t $fp, " << frame_size - 2 * WORD_LENGTH << "($sp)" << std::endl;
  // Restore stack pointer to the previous frame bottom.
  asm_out << "addiu\t $sp, $sp, " << frame_size << std::endl;
  // Jump to caller next instruction.
  asm_out << "j\t $ra" << std::endl;
}

void compileRootLevel(std::ofstream& asm_out, const Node* ast) {
  if (Util::DEBUG) {
    std::cerr << "==> Compiling root level." << std::endl;
  }

  // Global variable declaration.
  // TODO.

  // Function definition.
  if (ast->getType() == "FunctionDefinition") {
    const FunctionDefinition* function_definition =
      dynamic_cast<const FunctionDefinition*>(ast);
    compileFunctionDefinition(asm_out, function_definition);
  }
  // Unkonwn or unexpected node.
  else {
    if (Util::DEBUG) {
      std::cerr << "Unkown or unexpected node type: " << ast->getType() << std::endl;
    }
    Util::abort();
  }
}

void compileAst(const std::vector<const Node*>& ast_roots, std::ofstream& asm_out) {
  for (const Node* ast : ast_roots) {
    if(Util::DEBUG) {
      std::cerr << std::endl << std::endl
                << "============ AST ============" << std::endl;
      ast->print(std::cerr, "");
      std::cerr << std::endl << std::endl
                << "======== COMPILATION ========" << std::endl;
    }
    compileRootLevel(asm_out, ast);
  }
}

int compile(const std::string& source_file_name,
            const std::string& destination_file_name) {
  FILE* file_in;
  if (!(file_in = fopen(source_file_name.c_str(), "r"))) {
    std::cerr << "Cannot open source file: '" << source_file_name << "'." << std::endl;
    return 1;
  }
  // Set file Flex and Yacc will read from.
  yyset_in(file_in);

  // Prepare asm output file.
  std::ofstream asm_out;
  asm_out.open(destination_file_name);

  // Compile.
  std::vector<const Node*> ast_roots = parseAST();
  compileAst(ast_roots, asm_out);

  // Close the files.
  fclose(file_in);
  asm_out.close();
  return 0;
}
