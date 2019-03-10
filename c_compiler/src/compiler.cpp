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

void compileIfStatement(std::ofstream& asm_out, const IfStatement* if_statement,
                        FunctionContext& function_context,
                        RegisterAllocator& register_allocator);

void compileWhileStatement(std::ofstream& asm_out, const WhileStatement* while_statement,
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
    compileArithmeticOrLogicalExpression(asm_out, unary_expression->getUnaryExpression(),
                                         new_reg, function_context, register_allocator);
    // Prefix ++ operator (e.g. ++a).
    // Dest_reg contains the already incremented value.
    if (unary_expression->getUnaryType() == "++"){
      asm_out << "addiu\t " << dest_reg << ", " << new_reg <<", 1" << std::endl;
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
              << "\t# Prefix increment variable: " << variable_id << std::endl;
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
              << "\t# Prefix decrement variable: " << variable_id << std::endl;
    }
    // Unary operators not yet supported: & (address of), * (pointer dereference).
    // Unary + operator requires no action.

    // Unary minus operator.
    else if (unary_expression->getUnaryType() == "-"){
      asm_out << "subu\t " << dest_reg << ", $0, " << new_reg << std::endl;
    }
    // Unary not operator.
    else if (unary_expression->getUnaryType() == "~"){
      asm_out << "not\t " << dest_reg << ", " << new_reg << std::endl;
    }
    // Logical not operator.
    else if (unary_expression->getUnaryType() == "!"){
      asm_out << "sltiu\t " << dest_reg << ", " << new_reg << ", 1" << std::endl;
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
      asm_out << "addiu\t " << new_reg << ", " << new_reg <<", 1" << std::endl;
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
              << "\t# Postfix increment variable: " << variable_id << std::endl;
    }
    // Postfix -- operator (e.g. a--).
    // Dest_reg contains the value of a before it is decremented.
    else if (postfix_expression->getPostfixType() == "--"){
      asm_out << "move\t " << dest_reg << ", " << new_reg << std::endl;
      asm_out << "addiu\t " << new_reg << ", " << new_reg <<", -1" << std::endl;
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
              << "\t# Postfix decrement variable: " << variable_id << std::endl;
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
    }
    // Division.
    else if (multiplicative_expression->getMultiplicativeType() == "/"){
      asm_out << "divu\t " << dest_reg << ", " << rhs_reg << std::endl;
      asm_out << "mflo\t " << dest_reg << std::endl;
      asm_out << "nop" << std::endl;
    }
    // Modulo.
    else if (multiplicative_expression->getMultiplicativeType() == "%"){
      asm_out << "divu\t " << dest_reg << ", " << rhs_reg << std::endl;
      asm_out << "mfhi\t " << dest_reg << std::endl;
      asm_out << "nop" << std::endl;
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
      asm_out << "xori\t " << dest_reg << ", " << dest_reg << ", 1" << std::endl;
    }  
    // Greater or Equal.
    if (relational_expression->getRelationalType() == ">="){
      // compute less than.
      asm_out << "slt\t " << dest_reg << ", " << dest_reg << ", " <<rhs_reg  << std::endl;
      // greater or equal is the opposite of it.
      asm_out << "xori\t " << dest_reg << ", " << dest_reg << ", 1" << std::endl;
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
    std::string return_zero_id = CompilerUtil::makeUniqueId("return_zero");
    std::string end_and_id = CompilerUtil::makeUniqueId("end_and");
    
    asm_out << "beq\t " << dest_reg << ", $0, " << return_zero_id << std::endl;
    asm_out << "beq\t " << rhs_reg << ", $0, " << return_zero_id << std::endl;
    asm_out << "nop" << std::endl;
    asm_out << "li\t " << dest_reg << ", 1" << std::endl;
    asm_out << "b\t " << end_and_id << std::endl;
    asm_out << "nop" << std::endl;
    asm_out << return_zero_id << ":" << std::endl;
    asm_out << "move\t " << dest_reg << ", $0" << std::endl;
    asm_out << end_and_id << ":" << std::endl;

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
    
    asm_out << "bne\t " << dest_reg << ", $0, " << return_one_id << std::endl;
    asm_out << "bne\t " << rhs_reg << ", $0, " <<return_one_id << std::endl;
    asm_out << "nop" << std::endl;
    asm_out << "move\t " << dest_reg << ", $0" << std::endl;
    asm_out << "b\t " <<end_or_id << std::endl;
    asm_out << "nop" << std::endl;
    asm_out << return_one_id << ":" << std::endl;
    asm_out << "li\t " << dest_reg << ", 1" << std::endl;
    asm_out << end_or_id << ":" << std::endl;

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
    
    asm_out << "beq\t " << dest_reg << ", $0, " <<end_cond_id << std::endl;
    asm_out << "nop" << std::endl;
    compileArithmeticOrLogicalExpression(asm_out, conditional_expression->getExpression1(),
                                         exp1_reg, function_context, register_allocator);
    
    compileArithmeticOrLogicalExpression(asm_out, conditional_expression->getExpression2(),
                                         dest_reg, function_context, register_allocator);
    

    asm_out << "move\t " << dest_reg << ", " << exp1_reg << std::endl;
    asm_out << end_cond_id << ":" << std::endl;

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

// Inline compilation of a function call.
void compileFunctionCall(std::ofstream& asm_out, const FunctionCall* function_call, 
                         const std::string& dest_reg, FunctionContext& function_context,
                         RegisterAllocator& register_allocator) {
  if (Util::DEBUG) {
    std::cerr << "==> Compiling function call." << std::endl;
  }

  const std::string& function_id = function_call->getFunctionId();
  const ParametersListNode* parameters_list_node =
    dynamic_cast<const ParametersListNode*>(function_call->getParametersList());
  compileFunctionCallParametersList(asm_out, parameters_list_node, function_context,
                                    register_allocator);
  asm_out <<"jal " << function_id << std::endl;
}

void compileFunctionCallParametersList(std::ofstream& asm_out,
                                       const ParametersListNode* parameters_list_node,
                                       FunctionContext& function_context,
                                       RegisterAllocator& register_allocator) {
  if (Util::DEBUG) {
    std::cerr << "==> Compiling function call parameters list." << std::endl;
  }

  // Base cases.
  if (parameters_list_node->isEmptyParameterList()) {
    return;
  }
  else if(!parameters_list_node->hasNextParameter()) {
    // Only one parameter left.
    const Node* parameter = parameters_list_node->getParameter();
    std::string par_reg = register_allocator.requestFreeRegister();
    compileArithmeticOrLogicalExpression(asm_out, parameter, par_reg, function_context,
                                         register_allocator);
    register_allocator.freeRegister(par_reg);
  }
  // Recursive case.
  else if (parameters_list_node->hasNextParameter()) {
    // Parameter exists and has successor.
    const Node* parameter = parameters_list_node->getParameter();
    const ParametersListNode* next_parameter =
      dynamic_cast<const ParametersListNode*>(parameters_list_node->getNextParameter());
    std::string par_reg = register_allocator.requestFreeRegister();
    compileArithmeticOrLogicalExpression(asm_out, parameter, par_reg, function_context,
                                           register_allocator);
    compileFunctionCallParametersList(asm_out, next_parameter, function_context,
                                      register_allocator);
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
            << variable_id << std::endl;
    register_allocator.freeRegister(rhs_reg);
  } else {
    int offset = function_context.placeVariableInStack(variable_id);
    asm_out << "sw\t " << "$0, " << offset << "($fp)" << "\t# Declare variable: "
            << variable_id << std::endl;
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
            << variable_id << std::endl;
  }
  
  else if (assignment_expression->getAssignmentType() == "*="){
    int offset = function_context.getOffsetForVariable(variable_id); 
    asm_out << "lw\t " << var_reg << ", " << offset << "($fp)" << "\t# Retreive variable: "
            << variable_id << std::endl;
    asm_out << "multu\t " << var_reg << ", " << var_reg << ", " << tmp_reg << std::endl;
    asm_out << "mflo\t " << var_reg << std::endl;
    asm_out << "nop" << std::endl;
    asm_out << "sw\t " << var_reg << ", " << offset << "($fp)" << "\t# Assign variable: "
            << variable_id << std::endl;
  }

  else if (assignment_expression->getAssignmentType() == "/="){
    int offset = function_context.getOffsetForVariable(variable_id); 
    asm_out << "lw\t " << var_reg << ", " << offset << "($fp)" << "\t# Retreive variable: "
            << variable_id << std::endl;
    asm_out << "divu\t " << var_reg << ", " << var_reg << ", " << tmp_reg << std::endl;
    asm_out << "mflo\t " << var_reg << std::endl;
    asm_out << "nop" << std::endl;
    asm_out << "sw\t " << var_reg << ", " << offset << "($fp)" << "\t# Assign variable: "
            << variable_id << std::endl;
  }

  else if (assignment_expression->getAssignmentType() == "%="){
    int offset = function_context.getOffsetForVariable(variable_id); 
    asm_out << "lw\t " << var_reg << ", " << offset << "($fp)" << "\t# Retreive variable: "
            << variable_id << std::endl;
    asm_out << "divu\t " << var_reg << ", " << var_reg << ", " << tmp_reg << std::endl;
    asm_out << "mfhi\t " << var_reg << std::endl;
    asm_out << "nop" << std::endl;
    asm_out << "sw\t " << var_reg << ", " << offset << "($fp)" << "\t# Assign variable: "
            << variable_id << std::endl;
  }

  else if (assignment_expression->getAssignmentType() == "+="){
    int offset = function_context.getOffsetForVariable(variable_id); 
    asm_out << "lw\t " << var_reg << ", " << offset << "($fp)" << "\t# Retreive variable: "
            << variable_id << std::endl;
    asm_out << "addu\t " << var_reg << ", " << var_reg << ", " << tmp_reg << std::endl;
    asm_out << "sw\t " << var_reg << ", " << offset << "($fp)" << "\t# Assign variable: "
            << variable_id << std::endl;
  }

  else if (assignment_expression->getAssignmentType() == "-="){
    int offset = function_context.getOffsetForVariable(variable_id); 
    asm_out << "lw\t " << var_reg << ", " << offset << "($fp)" << "\t# Retreive variable: "
            << variable_id << std::endl;
    asm_out << "subu\t " << var_reg << ", " << var_reg << ", " << tmp_reg << std::endl;
    asm_out << "sw\t " << var_reg << ", " << offset << "($fp)" << "\t# Assign variable: "
            << variable_id << std::endl;
  }

  else if (assignment_expression->getAssignmentType() == "<<="){
    int offset = function_context.getOffsetForVariable(variable_id); 
    asm_out << "lw\t " << var_reg << ", " << offset << "($fp)" << "\t# Retreive variable: "
            << variable_id << std::endl;
    asm_out << "sllv\t " << var_reg << ", " << var_reg << ", " << tmp_reg << std::endl;
    asm_out << "sw\t " << var_reg << ", " << offset << "($fp)" << "\t# Assign variable: "
            << variable_id << std::endl;
  }

  else if (assignment_expression->getAssignmentType() == ">>="){
    int offset = function_context.getOffsetForVariable(variable_id); 
    asm_out << "lw\t " << var_reg << ", " << offset << "($fp)" << "\t# Retreive variable: "
            << variable_id << std::endl;
    asm_out << "slrv\t " << var_reg << ", " << var_reg << ", " << tmp_reg << std::endl;
    asm_out << "sw\t " << var_reg << ", " << offset << "($fp)" << "\t# Assign variable: "
            << variable_id << std::endl;
  }

  else if (assignment_expression->getAssignmentType() == "&="){
    int offset = function_context.getOffsetForVariable(variable_id); 
    asm_out << "lw\t " << var_reg << ", " << offset << "($fp)" << "\t# Retreive variable: "
            << variable_id << std::endl;
    asm_out << "and\t " << var_reg << ", " << var_reg << ", " << tmp_reg << std::endl;
    asm_out << "sw\t " << var_reg << ", " << offset << "($fp)" << "\t# Assign variable: "
            << variable_id << std::endl;
  }

  else if (assignment_expression->getAssignmentType() == "^="){
    int offset = function_context.getOffsetForVariable(variable_id); 
    asm_out << "lw\t " << var_reg << ", " << offset << "($fp)" << "\t# Retreive variable: "
            << variable_id << std::endl;
    asm_out << "xor\t " << var_reg << ", " << var_reg << ", " << tmp_reg << std::endl;
    asm_out << "sw\t " << var_reg << ", " << offset << "($fp)" << "\t# Assign variable: "
            << variable_id << std::endl;
  }

  else if (assignment_expression->getAssignmentType() == "|="){
    int offset = function_context.getOffsetForVariable(variable_id); 
    asm_out << "lw\t " << var_reg << ", " << offset << "($fp)" << "\t# Retreive variable: "
            << variable_id << std::endl;
    asm_out << "or\t " << var_reg << ", " << var_reg << ", " << tmp_reg << std::endl;
    asm_out << "sw\t " << var_reg << ", " << offset << "($fp)" << "\t# Assign variable: "
            << variable_id << std::endl;
  }

  register_allocator.freeRegister(tmp_reg);
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
    asm_out << "b " << function_context.getEpilogueLabel() << std::endl;
    register_allocator.freeRegister(dest_reg);
  } 
}

void compileIfStatement(std::ofstream& asm_out, const IfStatement* if_statement,
                        FunctionContext& function_context,
                        RegisterAllocator& register_allocator) {
  if (Util::DEBUG) {
    std::cerr << "==> Compiling if statement." << std::endl;
  }

  // Compile condition.
  std::string cond_reg = register_allocator.requestFreeRegister();
  compileArithmeticOrLogicalExpression(asm_out, if_statement->getCondition(), cond_reg,
                                       function_context, register_allocator);

  std::string top_else_id = CompilerUtil::makeUniqueId("top_else");
  asm_out << "beq\t " << cond_reg << ", $0, " << top_else_id << std::endl;
  asm_out << "nop" << std::endl;
  register_allocator.freeRegister(cond_reg);

  // Compile if body.
  // We could have a single statement (no brackets) or a compound statement.
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
  
  // If the body has been executed, than we need to jump the else.
  std::string end_if_id = CompilerUtil::makeUniqueId("end_if");
  asm_out << "b\t " << end_if_id << std::endl;
  asm_out << "nop" << std::endl;
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
  asm_out << "beq\t " << cond_reg << ", $0, " << end_while_id << std::endl;
  asm_out << "nop" << std::endl;

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

  asm_out << "b\t " << top_while_id << std::endl;
  asm_out << "nop" << std::endl;
  asm_out << end_while_id << ":" << std::endl;
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
  const std::string& epilogue_label = CompilerUtil::makeUniqueId(id + "_epilogue");

  // Create function context.
  FunctionContext function_context(frame_size, epilogue_label);

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
