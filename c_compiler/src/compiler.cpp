#include "../inc/compiler.hpp"
#include "../inc/compiler_util.hpp"

#include "../../common/inc/ast.hpp"
#include "../../common/inc/util.hpp"

#include <fstream>

#define WORD_LENGTH 4

void compileArithmeticOrLogicalExpression(std::ofstream& asm_out,
                                          const Node* arithmetic_or_logical_expression,
                                          const std::string& dest_reg, 
                                          FunctionContext& function_context,
                                          RegisterAllocator& register_allocator);

void compileFunctionCall(std::ofstream& asm_out, const FunctionCall* function_call, 
                         const std::string& dest_reg, FunctionContext& function_context,
                         RegisterAllocator& register_allocator); 

void compileFunctionCallParametersList(std::ofstream& asm_out,
                                       const ParametersListNode* parameters_list_node,
                                       int param_number,
                                       FunctionContext& function_context,
                                       RegisterAllocator& register_allocator);

void compileVariableDeclaration(std::ofstream& asm_out,
                                const DeclarationExpression* declaration_expression,
                                FunctionContext& function_context, 
                                RegisterAllocator& register_allocator);

void compileAssignmentExpression(std::ofstream& asm_out,
                                 const AssignmentExpression* assignment_expression,
                                 FunctionContext& function_context,
                                 RegisterAllocator& register_allocator);

void compileReturnStatement(std::ofstream& asm_out,
                            const ReturnStatement* return_statement,
                            FunctionContext& function_context,
                            RegisterAllocator& register_allocator);

void compileBreakStatement(std::ofstream& asm_out,
                            const BreakStatement* break_statement,
                            FunctionContext& function_context,
                            RegisterAllocator& register_allocator);

void compileContinueStatement(std::ofstream& asm_out,
                            const ContinueStatement* continue_statement,
                            FunctionContext& function_context,
                            RegisterAllocator& register_allocator);

void compileIfStatement(std::ofstream& asm_out, const IfStatement* if_statement,
                        FunctionContext& function_context,
                        RegisterAllocator& register_allocator);

void compileWhileStatement(std::ofstream& asm_out, const WhileStatement* while_statement,
                           FunctionContext& function_context, 
                           RegisterAllocator& register_allocator);

void compileForStatement(std::ofstream& asm_out, const ForStatement* for_statement,
                           FunctionContext& function_context, 
                           RegisterAllocator& register_allocator);

void compileSwitchStatement(std::ofstream& asm_out, 
                            const SwitchStatement* switch_statement,
                            FunctionContext& function_context, 
                            RegisterAllocator& register_allocator);

void compileCaseStatementList(std::ofstream& asm_out,
                              const CaseStatementListNode* case_statement_list_node,
                              const std::string& test_reg, const std::string& def_reg, 
                              FunctionContext& function_context, 
                              RegisterAllocator& register_allocator);

void compileCaseStatement(std::ofstream& asm_out,
                          const CaseStatement* case_statement, 
                          const std::string& test_reg, const std::string& def_reg,
                          FunctionContext& function_context, 
                          RegisterAllocator& register_allocator);

void compileDefaultStatement(std::ofstream& asm_out,
                             const DefaultStatement* default_statement, 
                             const std::string& def_reg,
                             FunctionContext& function_context, 
                             RegisterAllocator& register_allocator);

void compileStatement(std::ofstream& asm_out, const Node* statement,
                      FunctionContext& function_context, 
                      RegisterAllocator& register_allocator);

void compileStatementList(std::ofstream& asm_out,
                          const StatementListNode* statement_list_node, 
                          FunctionContext& function_context, 
                          RegisterAllocator& register_allocator); 

void compileFunctionDefinition(std::ofstream& asm_out,
                               const FunctionDefinition* function_definition,
                               RegisterAllocator& register_allocator);

void compileFunctionCallParametersList(std::ofstream& asm_out,
                                       ParametersListNode* parameters_list_node,
                                       FunctionContext& function_context,
                                       RegisterAllocator& register_allocator);

void compileRootLevel(std::ofstream& asm_out, const Node* ast, 
                      RegisterAllocator& register_allocator);

void compileAst(std::ofstream& asm_out, const std::vector<const Node*>& ast_roots,
                RegisterAllocator& register_allocator);

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
    
    // Add immediate constant into destination register.
    asm_out << "li\t " << dest_reg << ", " << integer_constant->getValue()
            << "\t# Add immediate constant into destination register." << std::endl;
  }
  else if (arithmetic_or_logical_expression->getType() == "Variable") {
    const Variable* variable =
      dynamic_cast<const Variable*>(arithmetic_or_logical_expression);
    int var_offset = function_context.getOffsetForVariable(variable->getId());
    
    asm_out << "lw\t " << dest_reg << ", " << var_offset << "($fp) \t# Load variable "
            << variable->getId() << " from the stack." << std::endl;
  }

  // Recursive cases.
  else if (arithmetic_or_logical_expression->getType() == "UnaryExpression") {
    const UnaryExpression* unary_expression =
      dynamic_cast<const UnaryExpression*>(arithmetic_or_logical_expression);
    std::string new_reg = register_allocator.requestFreeRegister();
    compileArithmeticOrLogicalExpression(asm_out, unary_expression->getUnaryExpression(),
                                         new_reg, function_context, register_allocator);
    // Prefix ++ operator (e.g. ++a).
    // Dest_reg contains the already incremented value.
    if (unary_expression->getUnaryType() == "++"){
      asm_out << "addiu\t " << dest_reg << ", " << new_reg  << ", 1 " << std::endl;
      
      if (unary_expression->getUnaryExpression()->getType() != "Variable") {
        if (Util::DEBUG) {
          std::cerr << "Non variable type used with ++ operator." << std::endl;
        }
        Util::abort();
      }
      const Variable* variable =
        dynamic_cast<const Variable*>(unary_expression->getUnaryExpression());
      const std::string& variable_id = variable->getId();
      int offset = function_context.getOffsetForVariable(variable_id);
      asm_out << "sw\t " << dest_reg << ", " << offset << "($fp)"
              << "\t# Prefix increment variable: " << variable_id << "." << std::endl;
    }
    // Prefix -- operator (e.g. --a).
    // Dest_reg contains the already decremented value.
    else if (unary_expression->getUnaryType() == "--"){
      asm_out << "addiu\t " << dest_reg << ", " << new_reg <<", -1" << std::endl;
      
      if (unary_expression->getUnaryExpression()->getType() != "Variable") {
        if (Util::DEBUG) {
          std::cerr << "Non variable type used with -- operator." << std::endl;
        }
        Util::abort();
      }
      const Variable* variable =
        dynamic_cast<const Variable*>(unary_expression->getUnaryExpression());
      const std::string& variable_id = variable->getId();
      int offset = function_context.getOffsetForVariable(variable_id);
      asm_out << "sw\t " << dest_reg << ", " << offset << "($fp)"
              << "\t# Prefix decrement variable: " << variable_id << "." << std::endl;
    }
    // Unary operators not yet supported: & (address of), * (pointer dereference).
    // Unary + operator requires no action.

    // Unary minus operator.
    else if (unary_expression->getUnaryType() == "-"){
      asm_out << "subu\t " << dest_reg << ", $0, " << new_reg 
              << "\t# Unary minus." << std::endl;
    }
    // Unary not operator.
    else if (unary_expression->getUnaryType() == "~"){
      asm_out << "not\t " << dest_reg << ", " << new_reg
              << "\t# Unary not." << std::endl;
    }
    // Logical not operator.
    else if (unary_expression->getUnaryType() == "!"){
      asm_out << "sltiu\t " << dest_reg << ", " << new_reg << ", 1"
              << "\t# Logical not." << std::endl;
    }

    register_allocator.freeRegister(new_reg);
  }

  else if (arithmetic_or_logical_expression->getType() == "PostfixExpression") {
    const PostfixExpression* postfix_expression =
      dynamic_cast<const PostfixExpression*>(arithmetic_or_logical_expression);
    std::string new_reg = register_allocator.requestFreeRegister();
    compileArithmeticOrLogicalExpression(asm_out, 
                                         postfix_expression->getPostfixExpression(),
                                         new_reg, function_context, register_allocator);
    // Postfix ++ operator (e.g. a++).
    // Dest_reg contains the value of a before it is incremented.
    if (postfix_expression->getPostfixType() == "++"){
      asm_out << "move\t " << dest_reg << ", " << new_reg << std::endl;
      asm_out << "addiu\t " << new_reg << ", " << new_reg << ", 1" << std::endl;
      if (postfix_expression->getPostfixExpression()->getType() != "Variable") {
        if (Util::DEBUG) {
          std::cerr << "Non variable type used with ++ operator." << std::endl;
        }
        Util::abort();
      }
      const Variable* variable =
        dynamic_cast<const Variable*>(postfix_expression->getPostfixExpression());
      const std::string& variable_id = variable->getId();
      int offset = function_context.getOffsetForVariable(variable_id);
      asm_out << "sw\t " << new_reg << ", " << offset << "($fp)"
              << "\t# Postfix increment variable: " << variable_id << "." << std::endl;
    }
    // Postfix -- operator (e.g. a--).
    // Dest_reg contains the value of a before it is decremented.
    else if (postfix_expression->getPostfixType() == "--"){
      asm_out << "move\t " << dest_reg << ", " << new_reg << std::endl;
      asm_out << "addiu\t " << new_reg << ", " << new_reg << ", -1" << std::endl;
      if (postfix_expression->getPostfixExpression()->getType() != "Variable") {
        if (Util::DEBUG) {
          std::cerr << "Non variable type used with -- operator." << std::endl;
        }
        Util::abort();
      }
      const Variable* variable =
        dynamic_cast<const Variable*>(postfix_expression->getPostfixExpression());
      const std::string& variable_id = variable->getId();
      int offset = function_context.getOffsetForVariable(variable_id);
      asm_out << "sw\t " << new_reg << ", " << offset << "($fp)"
              << "\t# Postfix decrement variable: " << variable_id << "." << std::endl;
    }

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
      asm_out << "nop" << std::endl;
      asm_out << "nop" << "\t# Multiplication." << std::endl;
    }
    // Division.
    else if (multiplicative_expression->getMultiplicativeType() == "/"){
      asm_out << "divu\t " << dest_reg << ", " << rhs_reg << std::endl;
      asm_out << "mflo\t " << dest_reg << std::endl;
      asm_out << "nop" << std::endl;
      asm_out << "nop" << "\t# Division." << std::endl;
    }
    // Modulo.
    else if (multiplicative_expression->getMultiplicativeType() == "%"){
      asm_out << "divu\t " << dest_reg << ", " << rhs_reg << std::endl;
      asm_out << "mfhi\t " << dest_reg << std::endl;
      asm_out << "nop" << std::endl;
      asm_out << "nop" << "\t# Modulo." << std::endl;
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
      asm_out << "addu\t " << dest_reg << ", " << dest_reg << ", " << rhs_reg 
              << "\t# Addition." << std::endl;
    }
    // Subtraction case
    if (additive_expression->getAdditiveType() == "-"){
      asm_out << "subu\t " << dest_reg << ", " << dest_reg << ", " << rhs_reg
              << "\t# Subtraction." << std::endl;
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
      asm_out << "sllv\t " << dest_reg << ", " << dest_reg << ", " << rhs_reg 
              << "\t# Left shift." << std::endl;
    }  
    // Right shift (>>).
    if (shift_expression->getShiftType() == ">>"){
      asm_out << "srlv\t " << dest_reg << ", " << dest_reg << ", " << rhs_reg  
              << "\t# Right shift." << std::endl;
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
      asm_out << "slt\t " << dest_reg << ", " << dest_reg << ", " << rhs_reg
              << "\t# Less than."  << std::endl;
    }  
    // Greater than.
    if (relational_expression->getRelationalType() == ">"){
      asm_out << "slt\t " << dest_reg << ", " << rhs_reg << ", " << dest_reg
              << "\t# Greater than." << std::endl;
    }  
    // Less or Equal.
    if (relational_expression->getRelationalType() == "<="){
      // compute greater than.
      asm_out << "slt\t " << dest_reg << ", " << rhs_reg << ", " <<dest_reg  << std::endl;
      // less or equal is the opposite of it.
      asm_out << "xori\t " << dest_reg << ", " << dest_reg << ", 1" 
              << "\t# Less or equal." << std::endl;
    }  
    // Greater or Equal.
    if (relational_expression->getRelationalType() == ">="){
      // compute less than.
      asm_out << "slt\t " << dest_reg << ", " << dest_reg << ", " <<rhs_reg  << std::endl;
      // greater or equal is the opposite of it.
      asm_out << "xori\t " << dest_reg << ", " << dest_reg << ", 1" 
              << "\t# Greater or equal." << std::endl;
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
      // This will give in dest_reg 0 if lhs == rhs, and !0 otherwise.
      asm_out << "xor\t " << dest_reg << ", " << dest_reg << ", " << rhs_reg << std::endl;
      // if in dest_reg there is anything exept from 0, return 0; return 1 otherwise.
      asm_out << "sltiu\t " << dest_reg << ", " << dest_reg << ", 1"  
              << "\t# Equal to." << std::endl;
    }  
    // Not equal to (!=).
    if (equality_expression->getEqualityType() == "!="){
      // This will give in dest_reg 0 if lhs == rhs, and !0 otherwise.
      asm_out << "xor\t " << dest_reg << ", " << dest_reg << ", " << rhs_reg << std::endl;
      // If in dest_reg there is anything except from 0, return 1; return 0 otherwise.
      asm_out << "sltu\t " << dest_reg << ", $0, " << dest_reg
              << "\t# Not equal to." << std::endl;
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
    

    asm_out << "and\t " << dest_reg << ", " << dest_reg << ", " << rhs_reg
            << "\t# Bitwise and." << std::endl;

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
    

    asm_out << "xor\t " << dest_reg << ", " << dest_reg << ", " << rhs_reg
            << "\t# Bitwise xor." << std::endl;

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
    

    asm_out << "or\t " << dest_reg << ", " << dest_reg << ", " << rhs_reg
            << "\t#Bitwise or." << std::endl;

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
    std::string return_zero_id = CompilerUtil::makeUniqueId("return_zero");
    std::string end_and_id = CompilerUtil::makeUniqueId("end_and");
    asm_out << "## Start of logical and ##" << std::endl;
    asm_out << "beq\t " << dest_reg << ", $0, " << return_zero_id << std::endl;
    asm_out << "beq\t " << rhs_reg << ", $0, " << return_zero_id << std::endl;
    asm_out << "nop" << std::endl;
    asm_out << "li\t " << dest_reg << ", 1" << std::endl;
    asm_out << "b\t " << end_and_id << std::endl;
    asm_out << "nop" << std::endl;
    asm_out << return_zero_id << ":" << std::endl;
    asm_out << "move\t " << dest_reg << ", $0" << std::endl;
    asm_out << end_and_id << ":" << std::endl;
    asm_out << "## End of logical and ##" << std::endl;

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
    std::string return_one_id = CompilerUtil::makeUniqueId("return_one");
    std::string end_or_id = CompilerUtil::makeUniqueId("end_or");
    
    asm_out << "## Start of logical or ##" << std::endl;
    asm_out << "bne\t " << dest_reg << ", $0, " << return_one_id << std::endl;
    asm_out << "bne\t " << rhs_reg << ", $0, " <<return_one_id << std::endl;
    asm_out << "nop" << std::endl;
    asm_out << "move\t " << dest_reg << ", $0" << std::endl;
    asm_out << "b\t " <<end_or_id << std::endl;
    asm_out << "nop" << std::endl;
    asm_out << return_one_id << ":" << std::endl;
    asm_out << "li\t " << dest_reg << ", 1" << std::endl;
    asm_out << end_or_id << ":" << std::endl;
    asm_out << "## End of logical or ##" << std::endl;

    register_allocator.freeRegister(rhs_reg);
  }

  else if (arithmetic_or_logical_expression->getType() == "ConditionalExpression") {
    const ConditionalExpression* conditional_expression =
      dynamic_cast<const ConditionalExpression*>(arithmetic_or_logical_expression);

    std::string exp1_reg = register_allocator.requestFreeRegister();

    compileArithmeticOrLogicalExpression(asm_out, conditional_expression->getCondition(),
                                         dest_reg, function_context, register_allocator);
    
    // if condition true --> return exp1, else --> return exp2.
    std::string end_cond_id = CompilerUtil::makeUniqueId("end_cond");
    
    asm_out << "## Start of conditional expression ##" << std::endl;
    asm_out << "beq\t " << dest_reg << ", $0, " <<end_cond_id << std::endl;
    asm_out << "nop" << std::endl;
    compileArithmeticOrLogicalExpression(asm_out, conditional_expression->getExpression1(),
                                         exp1_reg, function_context, register_allocator);
    
    compileArithmeticOrLogicalExpression(asm_out, conditional_expression->getExpression2(),
                                         dest_reg, function_context, register_allocator);
    

    asm_out << "move\t " << dest_reg << ", " << exp1_reg << std::endl;
    asm_out << end_cond_id << ":" << std::endl;
    asm_out << "## End of conditional expression ##" << std::endl;

    register_allocator.freeRegister(exp1_reg);
  }

  else if (arithmetic_or_logical_expression->getType() == "FunctionCall") {
    const FunctionCall* function_call =
      dynamic_cast<const FunctionCall*>(arithmetic_or_logical_expression);
    compileFunctionCall(asm_out, function_call, dest_reg, function_context, 
                        register_allocator);
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

void compileFunctionCall(std::ofstream& asm_out, const FunctionCall* function_call, 
                         const std::string& dest_reg, FunctionContext& function_context,
                         RegisterAllocator& register_allocator) {
  if (Util::DEBUG) {
    std::cerr << "==> Compiling function call." << std::endl;
  }

  asm_out << "## DEST_REG for ff: " << dest_reg << std::endl;
  const std::string& function_id = function_call->getFunctionId();
  const ParametersListNode* parameters_list_node =
    dynamic_cast<const ParametersListNode*>(function_call->getParametersList());
  compileFunctionCallParametersList(asm_out, parameters_list_node, /*param_number=*/0,
                                    function_context, register_allocator);
  // Store temporary registers to stack before performing call.
  const std::vector<std::string>& temporary_registers_in_use =
    register_allocator.get_temporary_registers_in_use();
  for (const std::string& temporary_register : temporary_registers_in_use){
    int offset = function_context.placeVariableInStack(temporary_register);
    asm_out << "sw\t " << temporary_register << ", " << offset << "($fp)"
            << "\t# Storing temporary register in stack: " << temporary_register << "."
            << std::endl;
  }

  asm_out << "jal\t " << function_id << "\t# Function call to: " << function_id << "."
          << std::endl;
  
  // Restore temporary registers from stack.
  for (const std::string& temporary_register : temporary_registers_in_use){
    int offset = function_context.getOffsetForVariable(temporary_register);
    asm_out << "lw\t " << temporary_register << ", " << offset << "($fp)"
            << "\t# Restoring temporary register from stack: " << temporary_register
            << "." << std::endl;
  }
  // Move return value into temp register.
  asm_out << "move\t " << dest_reg << ", $v0" << "\t# Save result of function call."
          << std::endl;
}

void compileFunctionCallParametersList(std::ofstream& asm_out,
                                       const ParametersListNode* parameters_list_node,
                                       int param_number,
                                       FunctionContext& function_context,
                                       RegisterAllocator& register_allocator) {
  if (Util::DEBUG) {
    std::cerr << "==> Compiling function call parameters list." << std::endl;
  }

  if (param_number >= 4) {
    if (Util::DEBUG) {
      std::cerr << "Function calls with more than 4 paramaters are not supported yet."
                << std::endl;
    }
    Util::abort();
  } 

  std::string param_register = "$a" + std::to_string(param_number);

  // Base cases.
  if (parameters_list_node->isEmptyParameterList()) {
    return;
  }
  else if(!parameters_list_node->hasNextParameter()) {
    // Only one parameter left.
    const Node* parameter = parameters_list_node->getParameter();
    compileArithmeticOrLogicalExpression(asm_out, parameter, param_register,
                                         function_context, register_allocator);
  }
  // Recursive case.
  else if (parameters_list_node->hasNextParameter()) {
    // Parameter exists and has successor.
    const Node* parameter = parameters_list_node->getParameter();
    const ParametersListNode* next_parameter =
      dynamic_cast<const ParametersListNode*>(parameters_list_node->getNextParameter());
    compileArithmeticOrLogicalExpression(asm_out, parameter, param_register,
                                         function_context, register_allocator);
    compileFunctionCallParametersList(asm_out, next_parameter, param_number + 1,
                                      function_context, register_allocator);
  }
}

void compileVariableDeclaration(std::ofstream& asm_out,
                                const DeclarationExpression* declaration_expression,
                                FunctionContext& function_context, 
                                RegisterAllocator& register_allocator) {
  if (Util::DEBUG) {
    std::cerr << "==> Compiling variable declaration." << std::endl;
  }

  // Only supported type is int so far.
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
    std::string rhs_reg = register_allocator.requestFreeRegister();
    compileArithmeticOrLogicalExpression(asm_out, declaration_expression->getRhs(),
                                         rhs_reg, function_context, register_allocator);
    int offset = function_context.placeVariableInStack(variable_id);
    asm_out << "sw\t " << rhs_reg << ", " << offset << "($fp)" << "\t# Declare variable: "
            << variable_id << "." << std::endl;
    register_allocator.freeRegister(rhs_reg);
  } else {
    int offset = function_context.placeVariableInStack(variable_id);
    asm_out << "sw\t " << "$0, " << offset << "($fp)" << "\t# Declare variable: "
            << variable_id << "." << std::endl;
  }
}

void compileAssignmentExpression(std::ofstream& asm_out,
                                 const AssignmentExpression* assignment_expression,
                                 FunctionContext& function_context,
                                 RegisterAllocator& register_allocator) {
  if (Util::DEBUG) {
    std::cerr << "==> Compiling assignment expression." << std::endl;
  }

  // Extract id.
  const Variable* variable =
      dynamic_cast<const Variable*>(assignment_expression->getVariable());
  const std::string& variable_id = variable->getId();
  
  std::string var_reg = register_allocator.requestFreeRegister();
  std::string tmp_reg = register_allocator.requestFreeRegister();
  compileArithmeticOrLogicalExpression(asm_out, assignment_expression->getRhs(), tmp_reg,
                                       function_context, register_allocator);
  
  if (assignment_expression->getAssignmentType() == "="){
    int offset = function_context.getOffsetForVariable(variable_id); 
    asm_out << "sw\t " << tmp_reg << ", " << offset << "($fp)" << "\t# Assign variable: "
            << variable_id << "." << std::endl;
  }
  
  else if (assignment_expression->getAssignmentType() == "*="){
    int offset = function_context.getOffsetForVariable(variable_id); 
    asm_out << "lw\t " << var_reg << ", " << offset << "($fp)"
            << "\t# Retreive variable: " << variable_id << "." << std::endl;
    asm_out << "multu\t " << var_reg << ", " << var_reg << ", " << tmp_reg << std::endl;
    asm_out << "mflo\t " << var_reg << std::endl;
    asm_out << "nop" << std::endl;
    asm_out << "sw\t " << var_reg << ", " << offset << "($fp)" << "\t# Assign variable: "
            << variable_id << "." << std::endl;
  }

  else if (assignment_expression->getAssignmentType() == "/="){
    int offset = function_context.getOffsetForVariable(variable_id); 
    asm_out << "lw\t " << var_reg << ", " << offset << "($fp)"
            << "\t# Retreive variable: " << variable_id << "." << std::endl;
    asm_out << "divu\t " << var_reg << ", " << var_reg << ", " << tmp_reg << std::endl;
    asm_out << "mflo\t " << var_reg << std::endl;
    asm_out << "nop" << std::endl;
    asm_out << "sw\t " << var_reg << ", " << offset << "($fp)" << "\t# Assign variable: "
            << variable_id << "." << std::endl;
  }

  else if (assignment_expression->getAssignmentType() == "%="){
    int offset = function_context.getOffsetForVariable(variable_id); 
    asm_out << "lw\t " << var_reg << ", " << offset << "($fp)"
            << "\t# Retreive variable: " << variable_id << "." << std::endl;
    asm_out << "divu\t " << var_reg << ", " << var_reg << ", " << tmp_reg << std::endl;
    asm_out << "mfhi\t " << var_reg << std::endl;
    asm_out << "nop" << std::endl;
    asm_out << "sw\t " << var_reg << ", " << offset << "($fp)" << "\t# Assign variable: "
            << variable_id << "." << std::endl;
  }

  else if (assignment_expression->getAssignmentType() == "+="){
    int offset = function_context.getOffsetForVariable(variable_id); 
    asm_out << "lw\t " << var_reg << ", " << offset << "($fp)"
            << "\t# Retreive variable: " << variable_id << "." << std::endl;
    asm_out << "addu\t " << var_reg << ", " << var_reg << ", " << tmp_reg << std::endl;
    asm_out << "sw\t " << var_reg << ", " << offset << "($fp)" << "\t# Assign variable: "
            << variable_id << "." << std::endl;
  }

  else if (assignment_expression->getAssignmentType() == "-="){
    int offset = function_context.getOffsetForVariable(variable_id); 
    asm_out << "lw\t " << var_reg << ", " << offset << "($fp)"
            << "\t# Retreive variable: " << variable_id << "." << std::endl;
    asm_out << "subu\t " << var_reg << ", " << var_reg << ", " << tmp_reg << std::endl;
    asm_out << "sw\t " << var_reg << ", " << offset << "($fp)" << "\t# Assign variable: "
            << variable_id << "." << std::endl;
  }

  else if (assignment_expression->getAssignmentType() == "<<="){
    int offset = function_context.getOffsetForVariable(variable_id); 
    asm_out << "lw\t " << var_reg << ", " << offset << "($fp)"
            << "\t# Retreive variable: " << variable_id << "." << std::endl;
    asm_out << "sllv\t " << var_reg << ", " << var_reg << ", " << tmp_reg << std::endl;
    asm_out << "sw\t " << var_reg << ", " << offset << "($fp)" << "\t# Assign variable: "
            << variable_id << "." << std::endl;
  }

  else if (assignment_expression->getAssignmentType() == ">>="){
    int offset = function_context.getOffsetForVariable(variable_id); 
    asm_out << "lw\t " << var_reg << ", " << offset << "($fp)"
            << "\t# Retreive variable: " << variable_id << "." << std::endl;
    asm_out << "slrv\t " << var_reg << ", " << var_reg << ", " << tmp_reg << std::endl;
    asm_out << "sw\t " << var_reg << ", " << offset << "($fp)" << "\t# Assign variable: "
            << variable_id << "." << std::endl;
  }

  else if (assignment_expression->getAssignmentType() == "&="){
    int offset = function_context.getOffsetForVariable(variable_id); 
    asm_out << "lw\t " << var_reg << ", " << offset << "($fp)"
            << "\t# Retreive variable: " << variable_id << "." << std::endl;
    asm_out << "and\t " << var_reg << ", " << var_reg << ", " << tmp_reg << std::endl;
    asm_out << "sw\t " << var_reg << ", " << offset << "($fp)" << "\t# Assign variable: "
            << variable_id << "." << std::endl;
  }

  else if (assignment_expression->getAssignmentType() == "^="){
    int offset = function_context.getOffsetForVariable(variable_id); 
    asm_out << "lw\t " << var_reg << ", " << offset << "($fp)"
            << "\t# Retreive variable: " << variable_id << "." << std::endl;
    asm_out << "xor\t " << var_reg << ", " << var_reg << ", " << tmp_reg << std::endl;
    asm_out << "sw\t " << var_reg << ", " << offset << "($fp)" << "\t# Assign variable: "
            << variable_id << "." << std::endl;
  }

  else if (assignment_expression->getAssignmentType() == "|="){
    int offset = function_context.getOffsetForVariable(variable_id); 
    asm_out << "lw\t " << var_reg << ", " << offset << "($fp)"
            << "\t# Retreive variable: " << variable_id << "." << std::endl;
    asm_out << "or\t " << var_reg << ", " << var_reg << ", " << tmp_reg << std::endl;
    asm_out << "sw\t " << var_reg << ", " << offset << "($fp)" << "\t# Assign variable: "
            << variable_id << "." << std::endl;
  }

  register_allocator.freeRegister(tmp_reg);
  register_allocator.freeRegister(var_reg);
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
    asm_out << "b " << function_context.getEpilogueLabel() 
            << "\t# Return statement." << std::endl;
    register_allocator.freeRegister(dest_reg);
  } 
}

void compileBreakStatement(std::ofstream& asm_out,
                            const BreakStatement* break_statement,
                            FunctionContext& function_context,
                            RegisterAllocator& register_allocator) {
  if (Util::DEBUG) {
    std::cerr << "==> Compiling break statement." << std::endl;
  }

  // jump outside last loop.
    asm_out << "b " << function_context.getEndLoopLabel() 
            <<"\t# Break statement." << std::endl;
}

void compileContinueStatement(std::ofstream& asm_out,
                            const ContinueStatement* continue_statement,
                            FunctionContext& function_context,
                            RegisterAllocator& register_allocator) {
  if (Util::DEBUG) {
    std::cerr << "==> Compiling continue statement." << std::endl;
  }

  // jump to top of the loop.
    asm_out << "b " << function_context.getStartLoopLabel() 
            <<"\t# Continue statement." << std::endl;
}

void compileIfStatement(std::ofstream& asm_out, const IfStatement* if_statement,
                        FunctionContext& function_context,
                        RegisterAllocator& register_allocator) {
  if (Util::DEBUG) {
    std::cerr << "==> Compiling if statement." << std::endl;
  }

  asm_out << "## If condition ##" << std::endl; 
  // Compile condition.
  std::string cond_reg = register_allocator.requestFreeRegister();
  compileArithmeticOrLogicalExpression(asm_out, if_statement->getCondition(), cond_reg,
                                       function_context, register_allocator);

  std::string top_else_id = CompilerUtil::makeUniqueId("top_else");
  asm_out << "beq\t " << cond_reg << ", $0, " << top_else_id << std::endl;
  asm_out << "nop" << std::endl;
  register_allocator.freeRegister(cond_reg);
  asm_out << "## End if condition ##" << std::endl;

  // Compile if body.
  // We could have a single statement (no brackets) or a compound statement.
  asm_out << "## If body ##" << std::endl;
  if (if_statement->getIfBody()->getType() == "StatementListNode") {
    // Compound statement (brackets).
    const StatementListNode* body =
      dynamic_cast<const StatementListNode*>(if_statement->getIfBody());

    compileStatementList(asm_out, body, function_context, register_allocator);
  } else {
    // Single statement (no brackets).
    compileStatement(asm_out, if_statement->getIfBody(), function_context, 
                     register_allocator);
  }
  
  // If the body has been executed, then we need to jump the else.
  std::string end_if_id = CompilerUtil::makeUniqueId("end_if");
  asm_out << "b\t " << end_if_id << "\t# Need to jump the else if executed the if body."
          << std::endl;
  asm_out << "nop" << std::endl;
  asm_out << "## End if body ##" << std::endl; 
  
  asm_out << "## Else body ##" << std::endl; 
  asm_out << top_else_id << ":" << std::endl;
  // Translate else body, if present.
  if (if_statement->hasElseBody()) {
    if (if_statement->getElseBody()->getType() == "StatementListNode") {
      // Compound statement (brackets).
      const StatementListNode* body =
        dynamic_cast<const StatementListNode*>(if_statement->getElseBody());

      compileStatementList(asm_out, body, function_context, register_allocator);
    } else {
      // Single statement (no brackets).
      compileStatement(asm_out, if_statement->getElseBody(), function_context,
                       register_allocator);
    }
  }
  // End of the statement label.

  asm_out << end_if_id << ":" << std::endl;
  asm_out << "## End else body and the whole if statement ##" << std::endl; 
}

void compileWhileStatement(std::ofstream& asm_out, const WhileStatement* while_statement,
                           FunctionContext& function_context, 
                           RegisterAllocator& register_allocator) {
  if (Util::DEBUG) {
    std::cerr << "==> Compile while statement." << std::endl;
  }
  
  std::string top_while_id = CompilerUtil::makeUniqueId("top_while");
  asm_out <<top_while_id << ":" << std::endl;
  
  // Compile condition.
  std::string cond_reg = register_allocator.requestFreeRegister();
  compileArithmeticOrLogicalExpression(asm_out, while_statement->getCondition(), cond_reg,
                                       function_context, register_allocator);

  std::string end_while_id = CompilerUtil::makeUniqueId("end_while");
  function_context.saveLoopLabels(top_while_id, end_while_id);

  asm_out << "beq\t " << cond_reg << ", $0, " << end_while_id
          << "\t# Checking the condition of the while." << std::endl; 
  asm_out << "nop" << std::endl;

  register_allocator.freeRegister(cond_reg);
  // Compile while body.
  // We could have a single statement (no brackets) or a compound statement.
  if (while_statement->getBody()->getType() == "StatementListNode") {
    // Compound statement (brackets).
    const StatementListNode* body =
      dynamic_cast<const StatementListNode*>(while_statement->getBody());
    compileStatementList(asm_out, body, function_context, register_allocator);
  } else {
    // Single statement (no brackets).
    compileStatement(asm_out, while_statement->getBody(), function_context,
                     register_allocator);
  }

  asm_out << "b\t " << top_while_id << "\t# Back to the start of the loop." << std::endl;
  asm_out << "nop" << std::endl;
  asm_out << end_while_id << ":" << std::endl;
  function_context.removeLoopLabels();
}

void compileForStatement(std::ofstream& asm_out, const ForStatement* for_statement,
                           FunctionContext& function_context, 
                           RegisterAllocator& register_allocator) {
  if (Util::DEBUG) {
    std::cerr << "==> Compile for statement." << std::endl;
  }
  
  // Compile init.
  compileStatement(asm_out, for_statement->getInit(), function_context, register_allocator);

  std::string top_for_id = CompilerUtil::makeUniqueId("top_for");
  asm_out <<top_for_id << ":" << std::endl;
  
  std::string top_increment_id = CompilerUtil::makeUniqueId("top_increment");

  // Compile condition.
  std::string cond_reg = register_allocator.requestFreeRegister();
  compileArithmeticOrLogicalExpression(asm_out, for_statement->getCondition(), cond_reg,
                                       function_context, register_allocator);

  std::string end_for_id = CompilerUtil::makeUniqueId("end_for");
  function_context.saveLoopLabels(top_increment_id, end_for_id);

  asm_out << "beq\t " << cond_reg << ", $0, " << end_for_id
          << "\t# Checking the condition of the for." << std::endl; 
  asm_out << "nop" << std::endl;

  register_allocator.freeRegister(cond_reg);

  // Compile for body.
  // We could have a single statement (no brackets) or a compound statement.
  if (for_statement->getBody()->getType() == "StatementListNode") {
    // Compound statement (brackets).
    const StatementListNode* body =
      dynamic_cast<const StatementListNode*>(for_statement->getBody());
    compileStatementList(asm_out, body, function_context, register_allocator);
  } else {
    // Single statement (no brackets).
    compileStatement(asm_out, for_statement->getBody(), function_context,
                     register_allocator);
  }

  // Compile increment
  asm_out << top_increment_id << ":  \t# Here jumps a continue statement. " << std::endl; 
  if ( for_statement->hasIncrement()){
    compileStatement(asm_out, for_statement->getIncrement(), function_context,
                     register_allocator);
  }
  
  asm_out << "b\t " << top_for_id << "\t# Back to the start of the loop." << std::endl;
  asm_out << "nop" << std::endl;
  asm_out << end_for_id << ":" << std::endl;
  function_context.removeLoopLabels();
}

void compileSwitchStatement(std::ofstream& asm_out, 
                            const SwitchStatement* switch_statement,
                            FunctionContext& function_context, 
                            RegisterAllocator& register_allocator) {
  if (Util::DEBUG) {
    std::cerr << "==> Compile switch statement." << std::endl;
  }
  
  std::string top_default_id = CompilerUtil::makeUniqueId("top_default");
  
  // Compile test.
  std::string test_reg = register_allocator.requestFreeRegister();
  compileArithmeticOrLogicalExpression(asm_out, switch_statement->getTest(), test_reg,
                                       function_context, register_allocator);

  std::string end_switch_id = CompilerUtil::makeUniqueId("end_switch");
  function_context.saveLoopLabels(top_default_id, end_switch_id);

  std::string def_reg = register_allocator.requestFreeRegister();
  asm_out << "li\t " << def_reg << ", 1 \t# initially default flag is set to one" 
          << std::endl;

  // Compile case statements.
  if (switch_statement->hasBody()) {
    // Compound statement (brackets).
    const CaseStatementListNode* body =
      dynamic_cast<const CaseStatementListNode*>(switch_statement->getBody());
    compileCaseStatementList(asm_out, body, test_reg, def_reg, 
                             function_context, register_allocator);
  } 
  asm_out <<"bne\t " << def_reg << ", $0, " << top_default_id
          << "\t# branch if default must be executed." << std::endl; 
  asm_out <<end_switch_id << ":" << std::endl;

  register_allocator.freeRegister(test_reg);
  register_allocator.freeRegister(def_reg);
  function_context.removeLoopLabels();
}

void compileCaseStatementList(std::ofstream& asm_out,
                          const CaseStatementListNode* case_statement_list_node,
                          const std::string& test_reg, const std::string& def_reg,
                          FunctionContext& function_context, 
                          RegisterAllocator& register_allocator) {
  if (Util::DEBUG) {
    std::cerr << "==> Compiling case statement list." << std::endl;
  }

  // Base cases.
  if (case_statement_list_node->isEmptyCaseStatementList()) {
    // Statement list is empty.
    return;
  }
  else if(!case_statement_list_node->hasNextCaseStatement()) {
    // Only one statement left.
    if(case_statement_list_node->getCaseStatement()->getType() == "CaseStatement"){
      const CaseStatement* case_statement =
        dynamic_cast<const CaseStatement*>(case_statement_list_node->getCaseStatement());
      compileCaseStatement(asm_out, case_statement, test_reg, def_reg, function_context,
                           register_allocator);
    }
    else if(case_statement_list_node->getCaseStatement()->getType() == "DefaultStatement"){
      const DefaultStatement* default_statement = 
        dynamic_cast<const DefaultStatement*>(case_statement_list_node->getCaseStatement());
      compileDefaultStatement(asm_out, default_statement, def_reg, 
                              function_context, register_allocator);
    }
    else {
      if (Util::DEBUG) {
        std::cerr << "Not a case nor a default statement is not allowed in a switch" 
                  << std::endl;
      }
      Util::abort();
    }

  }
  // Recursive case.
  else if (case_statement_list_node->hasNextCaseStatement()) {
    // Statement exists and has successor.
    if(case_statement_list_node->getCaseStatement()->getType() == "CaseStatement"){
      const CaseStatement* case_statement = 
        dynamic_cast<const CaseStatement*>(case_statement_list_node->getCaseStatement());
      compileCaseStatement(asm_out, case_statement, test_reg, def_reg, function_context,
                           register_allocator);
    }
    else if(case_statement_list_node->getCaseStatement()->getType() == "DefaultStatement"){
      const DefaultStatement* default_statement = 
        dynamic_cast<const DefaultStatement*>(case_statement_list_node->getCaseStatement());
      compileDefaultStatement(asm_out, default_statement, def_reg, 
                              function_context, register_allocator);
    }
    else {
      if (Util::DEBUG) {
        std::cerr << "Not a case nor a default statement is not allowed in a switch" 
                  << std::endl;
      }
      Util::abort();
    }
    const CaseStatementListNode* next_case_statement =
      dynamic_cast<const CaseStatementListNode*>(case_statement_list_node->getNextCaseStatement());
    compileCaseStatementList(asm_out, next_case_statement, test_reg, def_reg,
                             function_context, register_allocator);
  }
}

void compileCaseStatement(std::ofstream& asm_out,
                          const CaseStatement* case_statement, 
                          const std::string& test_reg, const std::string& def_reg,
                          FunctionContext& function_context, 
                          RegisterAllocator& register_allocator) {
  if (Util::DEBUG) {
    std::cerr << "==> Compiling case statement list." << std::endl;
  }

  std::string case_exp_reg = register_allocator.requestFreeRegister();
  std::string end_case_id = CompilerUtil::makeUniqueId("end_case");

  compileArithmeticOrLogicalExpression(asm_out, case_statement->getCaseExpr(), 
                                       case_exp_reg, function_context, register_allocator);
  asm_out <<"bne\t " << test_reg << ", " << case_exp_reg << ", " << end_case_id << std::endl;
  asm_out <<"move\t " << def_reg << ", $0 \t# default not to be executed" << std::endl;   
  
  register_allocator.freeRegister(case_exp_reg);  
  
  // body of the case statement
  if (case_statement->hasBody()){
    const StatementListNode* body =
      dynamic_cast<const StatementListNode*>(case_statement->getBody());
    compileStatementList(asm_out, body, function_context, register_allocator);
  }  
  asm_out << end_case_id << ":" << std::endl;
}

void compileDefaultStatement(std::ofstream& asm_out,
                          const DefaultStatement* default_statement, 
                          const std::string& def_reg,
                          FunctionContext& function_context, 
                          RegisterAllocator& register_allocator) {
  if (Util::DEBUG) {
    std::cerr << "==> Compiling default statement." << std::endl;
  }

  std::string end_default_id = CompilerUtil::makeUniqueId("end_default");
  asm_out <<"b\t "  << end_default_id << std::endl;
  std::string top_default_id = function_context.getDefaultLabel();
  asm_out << top_default_id <<":" << std::endl;

  // body of the default statement
  
  if (default_statement->hasBody()){
    const StatementListNode* body =
      dynamic_cast<const StatementListNode*>(default_statement->getBody());
    compileStatementList(asm_out, body, function_context, register_allocator);
  }
  asm_out << end_default_id << ":" << std::endl;
}


// Supported types of statement:
// - declaration expression
// - assignment expression
// - if else
// - while
// - return
// - any logical or arithmetic expression.
void compileStatement(std::ofstream& asm_out, const Node* statement,
                      FunctionContext& function_context, 
                      RegisterAllocator& register_allocator) {
  if (Util::DEBUG) {
    std::cerr << "==> Compiling statement." << std::endl;
  }

  const std::string& statement_type = statement->getType();
  
  if (statement_type == "DeclarationExpression") {
    const DeclarationExpression* declaration_expression =
      dynamic_cast<const DeclarationExpression*>(statement);
    compileVariableDeclaration(asm_out, declaration_expression, function_context,
                               register_allocator);
  }
  else if (statement_type == "AssignmentExpression") {
    const AssignmentExpression* assignment_expression =
      dynamic_cast<const AssignmentExpression*>(statement);
    compileAssignmentExpression(asm_out, assignment_expression, function_context,
                                register_allocator);
  }
  else if (statement_type == "ReturnStatement") {
    const ReturnStatement* return_statement =
      dynamic_cast<const ReturnStatement*>(statement);
    compileReturnStatement(asm_out, return_statement, function_context,
                           register_allocator);
  }
  else if (statement_type == "BreakStatement") {
    const BreakStatement* break_statement =
      dynamic_cast<const BreakStatement*>(statement);
    compileBreakStatement(asm_out, break_statement, function_context,
                           register_allocator);
  }
  else if (statement_type == "ContinueStatement") {
    const ContinueStatement* continue_statement =
      dynamic_cast<const ContinueStatement*>(statement);
    compileContinueStatement(asm_out, continue_statement, function_context,
                           register_allocator);
  }
  else if (statement_type == "IfStatement") {
    const IfStatement* if_statement =
      dynamic_cast<const IfStatement*>(statement);
    compileIfStatement(asm_out, if_statement, function_context,
                           register_allocator);
  }
  else if (statement_type == "WhileStatement") {
    const WhileStatement* while_statement =
      dynamic_cast<const WhileStatement*>(statement);
    compileWhileStatement(asm_out, while_statement, function_context,
                           register_allocator);
  }
  else if (statement_type == "ForStatement") {
    const ForStatement* for_statement =
      dynamic_cast<const ForStatement*>(statement);
    compileForStatement(asm_out, for_statement, function_context,
                           register_allocator);
  }
  else if (statement_type == "SwitchStatement") {
    const SwitchStatement* switch_statement =
      dynamic_cast<const SwitchStatement*>(statement);
    compileSwitchStatement(asm_out, switch_statement, function_context,
                           register_allocator);
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
           statement_type == "ConditionalExpression") {
    // Operation that do no return anything, e.g.:
    // a + b;
    // a++; note that this will actually change a, so we cannot just ignore this
    //      operations. They can have side effects.
    std::string tmp_reg = register_allocator.requestFreeRegister();
    compileArithmeticOrLogicalExpression(asm_out, statement, tmp_reg, function_context,
                                         register_allocator);
    register_allocator.freeRegister(tmp_reg);
  }
  else if (statement_type == "EmptyExpression"){}
  // Unkonwn or unexpected node.
  else {
    if (Util::DEBUG) {
      std::cerr << "Unknown or unexpected node type: " << statement->getType()
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
  // At least 6 words in each function frame.
  // See: https://minnie.tuhs.org/CompArch/Labs/week4.html section 3.5 
  int frame_size = bytes_to_allocate + 6 * WORD_LENGTH;
  // Add extra space in stack since we may have to store temporary register before a
  // function call. We only need 8 temporary registers for this compiler.
  frame_size += 8 * WORD_LENGTH;
  const std::string& epilogue_label = CompilerUtil::makeUniqueId(id + "_epilogue");

  // Create function context.
  FunctionContext function_context(frame_size, epilogue_label);

  asm_out << std::endl;
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

  std::vector<std::string> argument_names =
    CompilerUtil::getArgumentNamesFromFunctionDeclaration(argument_list_node);
  // Store the first 4 arguments of the function in the previous function frame.
  // Only if not main.
  if (id != "main") {
    asm_out << "sw\t $a0, " << 0 * WORD_LENGTH + frame_size << "($sp)" << std::endl;
    function_context.saveOffsetForArgument(argument_names.at(0),
                                           0 * WORD_LENGTH + frame_size);
    asm_out << "sw\t $a1, " << 1 * WORD_LENGTH + frame_size << "($sp)" << std::endl;
    function_context.saveOffsetForArgument(argument_names.at(1),
                                           1 * WORD_LENGTH + frame_size);
    asm_out << "sw\t $a2, " << 2 * WORD_LENGTH + frame_size << "($sp)" << std::endl;
    function_context.saveOffsetForArgument(argument_names.at(2),
                                           2 * WORD_LENGTH + frame_size);
    asm_out << "sw\t $a3, " << 3 * WORD_LENGTH + frame_size << "($sp)" << std::endl;
    function_context.saveOffsetForArgument(argument_names.at(3),
                                           3 * WORD_LENGTH + frame_size);
  }

  // Function body.
  asm_out << "## Body ##" << std::endl;
  compileStatementList(asm_out, statement_list_node, function_context,
                       register_allocator);

  // Function epilogue.
  asm_out << "## Epilogue ##" << std::endl;
  asm_out << epilogue_label << ":" << std::endl;
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
  asm_out << "nop" << std::endl;
  asm_out << std::endl;
}

void compileGlobalVariableDeclaration(
  std::ofstream& asm_out, const DeclarationExpression* declaration_expression,
  GlobalVariables& globals_variables) {
  if (Util::DEBUG) {
    std::cerr << "==> Compiling global variable declaration." << std::endl;
  }

  const std::string& type = declaration_expression->getTypeSpecifier();
  const Variable* variable =
    dynamic_cast<const Variable*>(declaration_expression->getVariable());
  const std::string& variable_info = variable->getInfo();
  const std::string& variable_id = variable->getId();

  globals_variables.addNewGlobalVariable(variable_id, variable_info);

  // Check type of the function. Only supported so far: int.
  if (type != "int") {
    if (Util::DEBUG) {
      std::cerr << "Unexpected global declaration with non-int type: " << type << "."
                << std::endl;
    }
    Util::abort();
  }

  // Normal variable (i.e. nor array, nor pointer etc...).
  if (variable_info == "normal") {
    // Integer is a full word in memory.
    if (declaration_expression->hasRhs()) {
      if (declaration_expression->getRhs()->getType() != "IntegerConstant"){
        if (Util::DEBUG) {
          std::cerr << "Only constants are supported so far as RHS of global variables "
                    << "declaration. " << std::endl;
        }
        Util::abort();
      }
      const IntegerConstant* integer_constant =
        dynamic_cast<const IntegerConstant*>(declaration_expression->getRhs());
      asm_out << variable_id << ": \t .word " << integer_constant->getValue()
              << "\t # Normal variable: " << variable_id << "." << std::endl;
    } else {
      // No constant value specified, initialize as zero.
      asm_out << variable_id << ": \t .word 0" << "\t # Normal variable: " << variable_id
              << "." << std::endl;
    }
  } else if (variable_info == "array") {
    if (Util::DEBUG) {
      std::cerr << "Currently no support for arrays." << std::endl;
    }
    Util::abort();
  }
}

void compileAst(std::ofstream& asm_out, const std::vector<const Node*>& ast_roots,
                RegisterAllocator& register_allocator,
                GlobalVariables& global_variables) {
  // Assembly output is made of two parts:
  // .data -> declares variable names used in program; storage allocated in main memory.
  // .text -> contains program code (instructions).
  // Reference: http://logos.cs.uic.edu/366/notes/mips%20quick%20tutorial.htm.

  // Abort if any unexpected node.
  for (const Node* ast : ast_roots) {
    if (ast->getType() != "FunctionDefinition" &&
        ast->getType() != "DeclarationExpression") {
      if (Util::DEBUG) {
        std::cerr << "Unkown or unexpected node type at root level: " << ast->getType()
                  << std::endl;
      }
      Util::abort();
    }
  }

  // Data.
  asm_out << "##################" << std::endl
          << "## Data section ##" << std::endl
          << "##################" << std::endl;
  asm_out << ".data" << std::endl;
  // Compile all global variable declarations.
  for (const Node* ast : ast_roots) {
    if(Util::DEBUG) {
      std::cerr << std::endl << std::endl
                << "============ AST ============" << std::endl;
      ast->print(std::cerr, "");
      std::cerr << std::endl << std::endl
                << "======== COMPILATION ========" << std::endl;
    }
    if (ast->getType() == "DeclarationExpression") {
      const DeclarationExpression* declaration_expression =
        dynamic_cast<const DeclarationExpression*>(ast);
      compileGlobalVariableDeclaration(asm_out, declaration_expression, global_variables);
    }
  }

  // Text.
  asm_out << std::endl;
  asm_out << "##################" << std::endl
          << "## Code section ##" << std::endl
          << "##################" << std::endl;
  asm_out << ".text" << std::endl;
  // Compile all functions definitions.
  for (const Node* ast : ast_roots) {
    if(Util::DEBUG) {
      std::cerr << std::endl << std::endl
                << "============ AST ============" << std::endl;
      ast->print(std::cerr, "");
      std::cerr << std::endl << std::endl
                << "======== COMPILATION ========" << std::endl;
    }
    if (ast->getType() == "FunctionDefinition") {
      const FunctionDefinition* function_definition =
        dynamic_cast<const FunctionDefinition*>(ast);
      compileFunctionDefinition(asm_out, function_definition, register_allocator);
    }
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

  // Prepare register allocator and global variables.
  RegisterAllocator register_allocator;
  GlobalVariables global_variables;
  // Compile.
  std::vector<const Node*> ast_roots = parseAST();
  compileAst(asm_out, ast_roots, register_allocator, global_variables);

  // Close the files.
  fclose(file_in);
  asm_out.close();
  return 0;
}
