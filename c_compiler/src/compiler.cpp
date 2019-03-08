#include "../inc/compiler.hpp"
#include "../inc/compiler_util.hpp"

#include "../../common/inc/ast.hpp"
#include "../../common/inc/util.hpp"

#include <fstream>

#define WORD_LENGTH 4

// Compilation of an arithmetic or logical expression.
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
void compileArithmeticOrLogicalExpression(std::ofstream& asm_out,
                                          const Node* arithmetic_or_logical_expression,
                                          const std::string& dest_reg, 
                                          FunctionContext& function_context,
                                          RegisterAllocator& register_allocator) {
  if (Util::DEBUG) {
    std::cerr << "==> Compiling arithmetic expression." << std::endl;
  }

  // Base cases.
  if (arithmetic_or_logical_expression->getType() == "IntegerConstant") {
    const IntegerConstant* integer_constant =
      dynamic_cast<const IntegerConstant*>(arithmetic_or_logical_expression);
    
    //add immediate constant into destination register
    asm_out << "li\t " << dest_reg << ", " << integer_constant->getValue() << std::endl;
  }
  else if (arithmetic_or_logical_expression->getType() == "Variable") {
    const Variable* variable =
      dynamic_cast<const Variable*>(arithmetic_or_logical_expression);
    int var_offset = function_context.getOffsetForVariable(variable->getId());
    
    asm_out << "lw\t " << dest_reg << ", " << var_offset << "($fp)" << std::endl;

  }

  // Recursive cases.
  else if (arithmetic_or_logical_expression->getType() == "UnaryExpression") {
    const UnaryExpression* unary_expression =
      dynamic_cast<const UnaryExpression*>(arithmetic_or_logical_expression);
    std::string new_reg = register_allocator.requestFreeRegister();
    compileArithmeticOrLogicalExpression(asm_out, unary_expression, new_reg,
                                         function_context, register_allocator);
    // ++ operator.
    if (unary_expression->getUnaryType() == "++"){
      asm_out << "addiu\t " << dest_reg << ", " << new_reg <<", 1" << std::endl;
    }
    // -- operator.
    else if (unary_expression->getUnaryType() == "--"){
      asm_out << "addiu\t " << dest_reg << ", " << new_reg <<", -1" << std::endl;
    }
    // Unary minus operator.
    else if (unary_expression->getUnaryType() == "-"){
      asm_out << "subu\t " << dest_reg << ", $0" << new_reg << std::endl;
    }
    //TODO OTHER CASES

    register_allocator.freeRegister(new_reg);
  }

  else if (arithmetic_or_logical_expression->getType() == "MultiplicativeExpression") {
    const MultiplicativeExpression* multiplicative_expression =
      dynamic_cast<const MultiplicativeExpression*>(arithmetic_or_logical_expression);

    compileArithmeticOrLogicalExpression(asm_out, multiplicative_expression->getLhs(), 
                                         dest_reg, function_context, register_allocator);
    
    std::string rhs_reg = register_allocator.requestFreeRegister();
    
    compileArithmeticOrLogicalExpression(asm_out, multiplicative_expression->getRhs(),
                                         rhs_reg, function_context, register_allocator);
    
    // Multiplication.
    if (multiplicative_expression->getMultiplicativeType() == "*"){
      asm_out << "multu\t " << dest_reg << ", " << rhs_reg << std::endl;
      asm_out << "mflo\t " << dest_reg << std::endl;
    }
    // Division.
    else if (multiplicative_expression->getMultiplicativeType() == "/"){
      asm_out << "divu\t " << dest_reg << ", " << rhs_reg << std::endl;
      asm_out << "mflo\t " << dest_reg << std::endl;
    }
    // Modulo.
    else if (multiplicative_expression->getMultiplicativeType() == "%"){
      asm_out << "divu\t " << dest_reg << ", " << rhs_reg << std::endl;
      asm_out << "mfhi\t " << dest_reg << std::endl;
    }
   
    register_allocator.freeRegister(rhs_reg);
  }

  else if (arithmetic_or_logical_expression->getType() == "AdditiveExpression") {
    const AdditiveExpression* additive_expression =
      dynamic_cast<const AdditiveExpression*>(arithmetic_or_logical_expression);

    compileArithmeticOrLogicalExpression(asm_out, additive_expression->getLhs(), dest_reg,
                                         function_context, register_allocator);
    
    std::string rhs_reg = register_allocator.requestFreeRegister();

    compileArithmeticOrLogicalExpression(asm_out, additive_expression->getRhs(), rhs_reg,
                                         function_context, register_allocator);
    
    // Addition case.
    if (additive_expression->getAdditiveType() == "+"){
      asm_out << "addu\t " << dest_reg << ", " << dest_reg << ", " << rhs_reg << std::endl;
    }
    // Subtraction case
    if (additive_expression->getAdditiveType() == "-"){
      asm_out << "subu\t " << dest_reg << ", " << dest_reg << ", " << rhs_reg << std::endl;
    }

    register_allocator.freeRegister(rhs_reg);
  }
  
  else if (arithmetic_or_logical_expression->getType() == "ShiftExpression") {
    const ShiftExpression* shift_expression =
      dynamic_cast<const ShiftExpression*>(arithmetic_or_logical_expression);
    
    compileArithmeticOrLogicalExpression(asm_out, shift_expression->getLhs(), dest_reg,
                                         function_context, register_allocator);
    
    std::string rhs_reg = register_allocator.requestFreeRegister();
    
    compileArithmeticOrLogicalExpression(asm_out, shift_expression->getRhs(), rhs_reg,
                                         function_context, register_allocator);
    // Left shift (<<).
    if (shift_expression->getShiftType() == "<<"){
      asm_out << "sllv\t " << dest_reg << ", " << dest_reg << ", " << rhs_reg << std::endl;
    }  
    // Right shift (>>).
    if (shift_expression->getShiftType() == ">>"){
      asm_out << "srlv\t " << dest_reg << ", " << dest_reg << ", " << rhs_reg  << std::endl;
    }  

    register_allocator.freeRegister(rhs_reg);
  }

  else if (arithmetic_or_logical_expression->getType() == "RelationalExpression") {
    const RelationalExpression* relational_expression =
      dynamic_cast<const RelationalExpression*>(arithmetic_or_logical_expression);
    
    compileArithmeticOrLogicalExpression(asm_out, relational_expression->getLhs(), dest_reg,
                                         function_context, register_allocator);
    
    std::string rhs_reg = register_allocator.requestFreeRegister();
    
    compileArithmeticOrLogicalExpression(asm_out, relational_expression->getRhs(), rhs_reg,
                                         function_context, register_allocator);
    // Less than.
    if (relational_expression->getRelationalType() == "<"){
      asm_out << "slt\t " << dest_reg << ", " << dest_reg << ", " <<rhs_reg  << std::endl;
    }  
    // Greater than.
    if (relational_expression->getRelationalType() == ">"){
      asm_out << "slt\t " << dest_reg << ", " << rhs_reg << ", " <<dest_reg  << std::endl;
    }  
    // Less or Equal.
    if (relational_expression->getRelationalType() == "<="){
      // compute greater than.
      asm_out << "slt\t " << dest_reg << ", " << rhs_reg << ", " <<dest_reg  << std::endl;
      // less or equal is the opposite of it.
      asm_out << "xori\t " << dest_reg << ", " << dest_reg << " 1" << std::endl;
    }  
    // Greater or Equal.
    if (relational_expression->getRelationalType() == "<="){
      // compute less than.
      asm_out << "slt\t " << dest_reg << ", " << dest_reg << ", " <<rhs_reg  << std::endl;
      // greater or equal is the opposite of it.
      asm_out << "xori\t " << dest_reg << ", " << dest_reg << " 1" << std::endl;
    }  

    register_allocator.freeRegister(rhs_reg);
  }

  else if (arithmetic_or_logical_expression->getType() == "EqualityExpression") {
    const EqualityExpression* equality_expression =
      dynamic_cast<const EqualityExpression*>(arithmetic_or_logical_expression);
    
    compileArithmeticOrLogicalExpression(asm_out, equality_expression->getLhs(), dest_reg,
                                         function_context, register_allocator);

    std::string rhs_reg = register_allocator.requestFreeRegister();

    compileArithmeticOrLogicalExpression(asm_out, equality_expression->getRhs(), rhs_reg,
                                         function_context, register_allocator);
    // Equal to (==).
    if (equality_expression->getEqualityType() == "=="){
      // this will give in dest_reg 0 if lhs == rhs, and !0 otherwise.
      asm_out << "xor\t " << dest_reg << ", " << dest_reg << ", " << rhs_reg << std::endl;
      // if in dest_reg there is anything exept from 0, return 0; return 1 otherwise.
      asm_out << "sltiu\t " << dest_reg << ", " << dest_reg << ", 1"  << std::endl;
    }  
    // Not equal to (!=).
    if (equality_expression->getEqualityType() == "!="){
      // this will give in dest_reg 0 if lhs == rhs, and !0 otherwise.
      asm_out << "xor\t " << dest_reg << ", " << dest_reg << ", " << rhs_reg << std::endl;
      // if in dest_reg there is anything exept from 0, return 1; return 0 otherwise.
      asm_out << "sltu\t " << dest_reg << ", $0, " << dest_reg  << std::endl;
    }  

    register_allocator.freeRegister(rhs_reg);
  }

  else if (arithmetic_or_logical_expression->getType() == "AndExpression") {
    const AndExpression* and_expression =
      dynamic_cast<const AndExpression*>(arithmetic_or_logical_expression);

    compileArithmeticOrLogicalExpression(asm_out, and_expression->getLhs(), dest_reg,
                                         function_context, register_allocator);

    std::string rhs_reg = register_allocator.requestFreeRegister();

    compileArithmeticOrLogicalExpression(asm_out, and_expression->getRhs(), rhs_reg,
                                         function_context, register_allocator);
    

    asm_out << "and\t " << dest_reg << ", " << dest_reg << ", " << rhs_reg << std::endl;

    register_allocator.freeRegister(rhs_reg);
  }

  else if (arithmetic_or_logical_expression->getType() == "ExclusiveOrExpression") {
    const ExclusiveOrExpression* exclusive_or_expression =
      dynamic_cast<const ExclusiveOrExpression*>(arithmetic_or_logical_expression);

    compileArithmeticOrLogicalExpression(asm_out, exclusive_or_expression->getLhs(),
                                         dest_reg, function_context, register_allocator);

    std::string rhs_reg = register_allocator.requestFreeRegister();

    compileArithmeticOrLogicalExpression(asm_out, exclusive_or_expression->getRhs(),
                                         rhs_reg, function_context, register_allocator);
    

    asm_out << "xor\t " << dest_reg << ", " << dest_reg << ", " << rhs_reg << std::endl;

    register_allocator.freeRegister(rhs_reg);
  }

  else if (arithmetic_or_logical_expression->getType() == "InclusiveOrExpression") {
    const InclusiveOrExpression* inclusive_or_expression =
      dynamic_cast<const InclusiveOrExpression*>(arithmetic_or_logical_expression);

    compileArithmeticOrLogicalExpression(asm_out, inclusive_or_expression->getLhs(),
                                         dest_reg, function_context, register_allocator);

    std::string rhs_reg = register_allocator.requestFreeRegister();

    compileArithmeticOrLogicalExpression(asm_out, inclusive_or_expression->getRhs(),
                                         rhs_reg, function_context, register_allocator);
    

    asm_out << "or\t " << dest_reg << ", " << dest_reg << ", " << rhs_reg << std::endl;

    register_allocator.freeRegister(rhs_reg);
  }

  else if (arithmetic_or_logical_expression->getType() == "LogicalAndExpression") {
    const LogicalAndExpression* logical_and_expression =
      dynamic_cast<const LogicalAndExpression*>(arithmetic_or_logical_expression);

    compileArithmeticOrLogicalExpression(asm_out, logical_and_expression->getLhs(),
                                         dest_reg, function_context, register_allocator);
    
    std::string rhs_reg = register_allocator.requestFreeRegister();
    
    compileArithmeticOrLogicalExpression(asm_out, logical_and_expression->getRhs(),
                                         rhs_reg, function_context, register_allocator);
    
    // if one of the operands is 0 --> return 0, else --> return 1.
    asm_out << "beq\t " << dest_reg << ", $0, return_zero" << std::endl;
    asm_out << "beq\t " << rhs_reg << ", $0, return_zero" << std::endl;
    asm_out << "li\t " << dest_reg << ", 1" << std::endl;
    asm_out << "b\t end_and" << std::endl;
    asm_out << "return_zero: " << std::endl;
    asm_out << "move\t " << dest_reg << ", $0" << std::endl;
    asm_out << "end_and: " << std::endl;

    register_allocator.freeRegister(rhs_reg);
  }

  else if (arithmetic_or_logical_expression->getType() == "LogicalOrExpression") {
    const LogicalOrExpression* logical_or_expression =
      dynamic_cast<const LogicalOrExpression*>(arithmetic_or_logical_expression);

    compileArithmeticOrLogicalExpression(asm_out, logical_or_expression->getLhs(),
                                         dest_reg, function_context, register_allocator);
    
    std::string rhs_reg = register_allocator.requestFreeRegister();
    
    compileArithmeticOrLogicalExpression(asm_out, logical_or_expression->getRhs(),
                                         rhs_reg, function_context, register_allocator);
    
    // if one of the operands is non 0 --> return 1, else --> return 0.
    asm_out << "bne\t " << dest_reg << ", $0, return_one" << std::endl;
    asm_out << "bne\t " << rhs_reg << ", $0, return_one" << std::endl;
    asm_out << "move\t " << dest_reg << ", $0" << std::endl;
    asm_out << "b\t end_or" << std::endl;
    asm_out << "return_one: " << std::endl;
    asm_out << "li\t " << dest_reg << ", 1" << std::endl;
    asm_out << "end_or: " << std::endl;

    register_allocator.freeRegister(rhs_reg);
  }

  else if (arithmetic_or_logical_expression->getType() == "ConditionalExpression") {
    const ConditionalExpression* conditional_expression =
      dynamic_cast<const ConditionalExpression*>(arithmetic_or_logical_expression);

    std::string exp1_reg = register_allocator.requestFreeRegister();
    std::string exp2_reg = register_allocator.requestFreeRegister();

    compileArithmeticOrLogicalExpression(asm_out, conditional_expression->getCondition(),
                                         dest_reg, function_context, register_allocator);
    compileArithmeticOrLogicalExpression(asm_out, conditional_expression->getExpression1(),
                                         exp1_reg, function_context, register_allocator);
    compileArithmeticOrLogicalExpression(asm_out, conditional_expression->getExpression2(),
                                         exp2_reg, function_context, register_allocator);
    
    // if condition true --> return exp1, else --> return exp2.
    asm_out << "beq\t " << dest_reg << ", $0, exp1" << std::endl;
    asm_out << "move\t " << dest_reg << ", " << exp2_reg << std::endl;
    asm_out << "b\t end_condition" << std::endl;
    asm_out << "exp1: " << std::endl;
    asm_out << "move\t " << dest_reg << ", " << exp1_reg << std::endl;
    asm_out << "end_condition: " << std::endl;

    register_allocator.freeRegister(exp1_reg);
    register_allocator.freeRegister(exp2_reg);
  }

  // Unknown or unexpected node.
  else {
    if (Util::DEBUG) {
      std::cerr << "Unkown or unexpected node type: "
                << arithmetic_or_logical_expression->getType() << std::endl;
    }
    Util::abort();
  }
}


void compileReturnStatement(std::ofstream& asm_out,
                            const ReturnStatement* return_statement,
                            FunctionContext& function_context,
                            RegisterAllocator& register_allocator) {
  if (Util::DEBUG) {
    std::cerr << "==> Compiling return statement." << std::endl;
  }

  if (return_statement->hasExpression()) {

    std::string dest_reg = register_allocator.requestFreeRegister();
    compileArithmeticOrLogicalExpression(asm_out, return_statement->getExpression(),
                                         dest_reg, function_context, register_allocator);
    
    // move return value in $2.
    asm_out << "move\t $v0, " << dest_reg <<std::endl;
    register_allocator.freeRegister(dest_reg);
  } 
}

// Supported types of statement:
// - declaration expression
// - assignment expression
// - if else
// - while
// - return
void compileStatement(std::ofstream& asm_out, const Node* statement,
                      FunctionContext& function_context, 
                      RegisterAllocator& register_allocator) {
  if (Util::DEBUG) {
    std::cerr << "==> Compiling statement." << std::endl;
  }

  const std::string& statement_type = statement->getType();
  
  if (statement_type == "ReturnStatement") {
    const ReturnStatement* return_statement =
      dynamic_cast<const ReturnStatement*>(statement);
    compileReturnStatement(asm_out, return_statement, function_context,
                           register_allocator);
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
                          const StatementListNode* statement_list_node, 
                          FunctionContext& function_context, 
                          RegisterAllocator& register_allocator) {
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
    compileStatement(asm_out, statement, function_context, register_allocator);
  }
  // Recursive case.
  else if (statement_list_node->hasNextStatement()) {
    // Statement exists and has successor.
    const Node* statement = statement_list_node->getStatement();
    const StatementListNode* next_statement =
      dynamic_cast<const StatementListNode*>(statement_list_node->getNextStatement());
    compileStatement(asm_out, statement, function_context, register_allocator);
    compileStatementList(asm_out, next_statement, function_context, register_allocator);
  }
}

void compileFunctionDefinition(std::ofstream& asm_out,
                               const FunctionDefinition* function_definition,
                               RegisterAllocator& register_allocator) {
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
  int bytes_to_allocate =
    CompilerUtil::countBytesForDeclarationsInFunction(function_definition);
  // 6 words in each function frame.
  // See: https://minnie.tuhs.org/CompArch/Labs/week4.html section 3.5 
  int frame_size = bytes_to_allocate + 6 * WORD_LENGTH;

  // Create function context
  FunctionContext function_context(frame_size);

  asm_out << "#### Function: " << id << " ####" << std::endl;

  // Function prologue.
  asm_out << "## Prologue ##" << std::endl;
  asm_out << ".globl\t " << id << std::endl;
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
  compileStatementList(asm_out, statement_list_node, function_context,
                       register_allocator);

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

void compileRootLevel(std::ofstream& asm_out, const Node* ast, 
                      RegisterAllocator& register_allocator) {
  if (Util::DEBUG) {
    std::cerr << "==> Compiling root level." << std::endl;
  }

  // Global variable declaration.
  // TODO.

  // Function definition.
  if (ast->getType() == "FunctionDefinition") {
    const FunctionDefinition* function_definition =
      dynamic_cast<const FunctionDefinition*>(ast);
    compileFunctionDefinition(asm_out, function_definition, register_allocator);
  }
  // Unkonwn or unexpected node.
  else {
    if (Util::DEBUG) {
      std::cerr << "Unkown or unexpected node type: " << ast->getType() << std::endl;
    }
    Util::abort();
  }
}

void compileAst(std::ofstream& asm_out, const std::vector<const Node*>& ast_roots,
                RegisterAllocator& register_allocator) {
  for (const Node* ast : ast_roots) {
    if(Util::DEBUG) {
      std::cerr << std::endl << std::endl
                << "============ AST ============" << std::endl;
      ast->print(std::cerr, "");
      std::cerr << std::endl << std::endl
                << "======== COMPILATION ========" << std::endl;
    }
    compileRootLevel(asm_out, ast, register_allocator);
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

  // Prepare register allocator.
  RegisterAllocator register_allocator;
  // Compile.
  std::vector<const Node*> ast_roots = parseAST();
  compileAst(asm_out, ast_roots, register_allocator);

  // Close the files.
  fclose(file_in);
  asm_out.close();
  return 0;
}
