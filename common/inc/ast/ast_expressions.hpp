#ifndef ast_expressions_hpp
#define ast_expressions_hpp

#include <string>
#include <iostream>

#include "ast_node.hpp"

class DeclarationExpression : public Node {
 private:
  std::string type_specifier_;
  const Node* variable_;
  const Node* rhs_;

 public:
  // Plain declaration.
  DeclarationExpression(const std::string& type_specifier, const Node* variable)
    : type_specifier_(type_specifier), variable_(variable), rhs_(nullptr) {
    type_ = "DeclarationExpression";
  }

  // Declaration with assignment.
  DeclarationExpression(const std::string& type_specifier, const Node* variable,
                        const Node* rhs)
    : type_specifier_(type_specifier), variable_(variable), rhs_(rhs) {
    type_ = "DeclarationExpression";
  }

  const std::string& getTypeSpecifer() const {
    return type_specifier_;
  }

  const Node* getVariable() const {
    return variable_;
  }

  const Node* getRhs() const {
    return rhs_;
  }

  virtual std::ostream& print(std::ostream& os, std::string indent) const override {
    os << indent << type_ << " [ " << type_specifier_ << std::endl;
    variable_->print(os, indent + "  ");
    if (rhs_ != nullptr) {
      os << std::endl;
      rhs_->print(os, indent + "  ");
    }
    os << std::endl << indent << "]";
    return os;
  }
};

class PostfixExpression : public Node {
 private:
  const Node* postfix_expression_;
  std::string postfix_type_;

 public:
  // Plain declaration.
  PostfixExpression(const Node* postfix_expression, std::string postfix_type)
    : postfix_expression_(postfix_expression), postfix_type_(postfix_type) {
    type_ = "PostfixExpression";
  }

  const Node* getPostfixExpression() const {
    return postfix_expression_;
  }

  std::string getPostfixType() const {
    return postfix_type_;
  }
  
  virtual std::ostream& print(std::ostream& os, std::string indent) const override {
    os << indent << type_ << " [" << std::endl;
    os << indent << "  type: " << postfix_type_ << std::endl;
    postfix_expression_->print(os, indent + "  ");
    os << std::endl << indent << "]";
    return os;
  }
};

class UnaryExpression : public Node {
 private:
  std::string unary_type_;
  const Node* unary_expression_;

 public:
  // Plain declaration.
  UnaryExpression(std::string unary_type, const Node* unary_expression)
    : unary_type_(unary_type), unary_expression_(unary_expression) {
    type_ = "UnaryExpression";
  }

  std::string getUnaryType() const {
    return unary_type_;
  }

  const Node* getUnaryExpression() const {
    return unary_expression_;
  }
  
  virtual std::ostream& print(std::ostream& os, std::string indent) const override {
    os << indent << type_ << " [" << std::endl;
    os << indent << "  type: " << unary_type_ << std::endl;
    unary_expression_->print(os, indent + "  ");
    os << std::endl << indent << "]";
    return os;
  }
};

class MultiplicativeExpression : public Node {
 private:
  const Node* multiplicative_expression_;
  std::string multiplicative_type_;
  const Node* unary_expression_;

 public:
  // Plain declaration.
  MultiplicativeExpression(const Node* multiplicative_expression, 
                           std::string multiplicative_type, const Node* unary_expression)
    : multiplicative_expression_(multiplicative_expression), 
    multiplicative_type_(multiplicative_type), unary_expression_(unary_expression) {
    type_ = "MultiplicativeExpression";
  }

  const Node* getMultiplicativeExpression() const {
    return multiplicative_expression_;
  }
  std::string getMultiplicativeType() const {
    return multiplicative_type_;
  }

  const Node* getUnaryExpression() const {
    return unary_expression_;
  }
  
  virtual std::ostream& print(std::ostream& os, std::string indent) const override {
    os << indent << type_ << " [" << std::endl;
    multiplicative_expression_->print(os, indent + "  ");
    os << std::endl;
    os << indent << "  type: " << multiplicative_type_ << std::endl;
    unary_expression_->print(os, indent + "  ");
    os << std::endl << indent << "]";
    return os;
  }
};

class AdditiveExpression : public Node {
 private:
  const Node* additive_expression_;
  std::string additive_type_;
  const Node* multiplicative_expression_;

 public:
  // Plain declaration.
  AdditiveExpression(const Node* additive_expression, std::string additive_type, 
                     const Node* multiplicative_expression)
    : additive_expression_(additive_expression), additive_type_(additive_type), 
    multiplicative_expression_(multiplicative_expression) {
    type_ = "AdditiveExpression";
  }

  const Node* getAdditiveExpression() const {
    return additive_expression_;
  }

  std::string getAdditiveType() const {
    return additive_type_;
  }

  const Node* getMultiplicativeExpression() const {
    return multiplicative_expression_;
  }

  virtual std::ostream& print(std::ostream& os, std::string indent) const override {
    os << indent << type_ << " [" << std::endl;
    additive_expression_->print(os, indent + "  ");
    os << std::endl;
    os << indent << "  type: " << additive_type_ << std::endl;
    multiplicative_expression_->print(os, indent + "  ");
    os << std::endl << indent << "]";
    return os;
  }
};

class ShiftExpression : public Node {
 private:
  const Node* shift_expression_;
  std::string shift_type_;
  const Node* additive_expression_;

 public:
  // Plain declaration.
  ShiftExpression(const Node* shift_expression, std::string shift_type,
                  const Node* additive_expression)
    : shift_expression_(shift_expression), shift_type_(shift_type),
    additive_expression_(additive_expression) {
    type_ = "ShiftExpression";
  }

  const Node* getShiftExpression() const {
    return shift_expression_;
  }

  std::string getShiftType() const {
    return shift_type_;
  }

  const Node* getAdditiveExpression() const {
    return additive_expression_;
  }

  virtual std::ostream& print(std::ostream& os, std::string indent) const override {
    os << indent << type_ << " [" << std::endl;
    shift_expression_->print(os, indent + "  ");
    os << std::endl;
    os << indent << "  type: " << shift_type_ << std::endl;
    additive_expression_->print(os, indent + "  ");
    os << std::endl << indent << "]";
    return os;
  }
};

class RelationalExpression : public Node {
 private:
  const Node* relational_expression_;
  std::string relational_type_;
  const Node* shift_expression_;

 public:
  // Plain declaration.
  RelationalExpression(const Node* relational_expression, std::string relational_type, 
                       const Node* shift_expression)
    : relational_expression_(relational_expression), relational_type_(relational_type), 
    shift_expression_(shift_expression) {
    type_ = "RelationalExpression";
  }

  const Node* getRelationalExpression() const {
    return relational_expression_;
  }

  std::string getRelationalType() const {
    return relational_type_;
  }

  const Node* getShiftExpression() const {
    return shift_expression_;
  }

  virtual std::ostream& print(std::ostream& os, std::string indent) const override {
    os << indent << type_ << " [" << std::endl;
    relational_expression_->print(os, indent + "  ");
    os << std::endl;
    os << indent << "  type: " << relational_type_ << std::endl;
    shift_expression_->print(os, indent + "  ");
    os << std::endl << indent << "]";
    return os;
  }
};

class EqualityExpression : public Node {
 private:
  const Node* equality_expression_;
  std::string equality_type_;
  const Node* relational_expression_;

 public:
  // Plain declaration.
  EqualityExpression(const Node* equality_expression, std::string equality_type,
                     const Node* relational_expression)
    : equality_expression_(equality_expression), equality_type_(equality_type),
    relational_expression_(relational_expression) {
    type_ = "EqualityExpression";
  }

  const Node* getEqualityExpression() const {
    return equality_expression_;
  }

  std::string getEqualityType() const {
    return equality_type_;
  }

  const Node* getRelationalExpression() const {
    return relational_expression_;
  }

  virtual std::ostream& print(std::ostream& os, std::string indent) const override {
    os << indent << type_ << " [" << std::endl;
    equality_expression_->print(os, indent + "  ");
    os << std::endl;
    os << indent << "  type: " << equality_type_ << std::endl;
    relational_expression_->print(os, indent + "  ");
    os << std::endl << indent << "]";
    return os;
  }
};

class AndExpression : public Node {
 private:
  const Node* and_expression_;
  const Node* equality_expression_;

 public:
  // Plain declaration.
  AndExpression(const Node* and_expression, const Node* equality_expression)
    : and_expression_(and_expression), equality_expression_(equality_expression) {
    type_ = "AndExpression";
  }

  const Node* getAndExpression() const {
    return and_expression_;
  }

  const Node* getEqualityExpression() const {
    return equality_expression_;
  }

  virtual std::ostream& print(std::ostream& os, std::string indent) const override {
    os << indent << type_ << " [" << std::endl;
    and_expression_->print(os, indent + "  ");
    os << std::endl;
    os << indent << "  type: " << " & "  << std::endl;
    equality_expression_->print(os, indent + "  ");
    os << std::endl << indent << "]";
    return os;
  }
};

class ExclusiveOrExpression : public Node {
 private:
  const Node* exclusive_or_expression_;
  const Node* and_expression_;

 public:
  // Plain declaration.
  ExclusiveOrExpression(const Node* exclusive_or_expression, const Node* and_expression)
    : exclusive_or_expression_(exclusive_or_expression), and_expression_(and_expression) {
    type_ = "ExclusiveOrExpression";
  }

  const Node* getExclusiveOrExpression() const {
    return exclusive_or_expression_;
  }

  const Node* getAndExpression() const {
    return and_expression_;
  }

  virtual std::ostream& print(std::ostream& os, std::string indent) const override {
    os << indent << type_ << " [" << std::endl;
    exclusive_or_expression_->print(os, indent + "  ");
    os << std::endl;
    os << indent << "  type: " << " ^ "  << std::endl;
    and_expression_->print(os, indent + "  ");
    os << std::endl << indent << "]";
    return os;
  }
};

class InclusiveOrExpression : public Node {
 private:
  const Node* inclusive_or_expression_;
  const Node* exclusive_or_expression_;

 public:
  // Plain declaration.
  InclusiveOrExpression(const Node* inclusive_or_expression, const Node* exclusive_or_expression)
    : inclusive_or_expression_(inclusive_or_expression), exclusive_or_expression_(exclusive_or_expression) {
    type_ = "InclusiveOrExpression";
  }

  const Node* getInclusiveOrExpression() const {
    return inclusive_or_expression_;
  }

  const Node* getExclusiveOrExpression() const {
    return exclusive_or_expression_;
  }

  virtual std::ostream& print(std::ostream& os, std::string indent) const override {
    os << indent << type_ << " [" << std::endl;
    inclusive_or_expression_->print(os, indent + "  ");
    os << std::endl;
    os << indent << "  type: " << " | "  << std::endl;
    exclusive_or_expression_->print(os, indent + "  ");
    os << std::endl << indent << "]";
    return os;
  }
};

class LogicalAndExpression : public Node {
 private:
  const Node* logical_and_expression_;
  const Node* inclusive_or_expression_;

 public:
  // Plain declaration.
  LogicalAndExpression(const Node* logical_and_expression, const Node* inclusive_or_expression)
    : logical_and_expression_(logical_and_expression), inclusive_or_expression_(inclusive_or_expression) {
    type_ = "LogicalAndExpression";
  }

  const Node* getLogicalAndExpression() const {
    return logical_and_expression_;
  }

  const Node* getInclusiveOrExpression() const {
    return inclusive_or_expression_;
  }

  virtual std::ostream& print(std::ostream& os, std::string indent) const override {
    os << indent << type_ << " [" << std::endl;
    logical_and_expression_->print(os, indent + "  ");
    os << std::endl;
    os << indent << "  type: " << " && "  << std::endl;
    inclusive_or_expression_->print(os, indent + "  ");
    os << std::endl << indent << "]";
    return os;
  }
};

class LogicalOrExpression : public Node {
 private:
  const Node* logical_or_expression_;
  const Node* logical_and_expression_;

 public:
  // Plain declaration.
  LogicalOrExpression(const Node* logical_or_expression, const Node* logical_and_expression)
    : logical_or_expression_(logical_or_expression), logical_and_expression_(logical_and_expression) {
    type_ = "LogicalOrExpression";
  }

  const Node* getLogicalOrExpression() const {
    return logical_or_expression_;
  }

  const Node* getLogicalAndExpression() const {
    return logical_and_expression_;
  }

  virtual std::ostream& print(std::ostream& os, std::string indent) const override {
    os << indent << type_ << " [" << std::endl;
    logical_or_expression_->print(os, indent + "  ");
    os << std::endl;
    os << indent << "  type: " << " || "  << std::endl;
    logical_and_expression_->print(os, indent + "  ");
    os << std::endl << indent << "]";
    return os;
  }
};

class ConditionalExpression : public Node {
 private:
  const Node* logical_or_expression_;
  const Node* expression_;
  const Node* conditional_expression_;

 public:
  // Plain declaration.
  ConditionalExpression(const Node* logical_or_expression, const Node* expression, const Node* conditional_expression)
    : logical_or_expression_(logical_or_expression), expression_(expression), conditional_expression_(conditional_expression) {
    type_ = "ConditionalExpression";
  }

  const Node* getLogicalOrExpression() const {
    return logical_or_expression_;
  }

  const Node* getExpression() const {
    return expression_;
  }

  const Node* getConditionalExpression() const {
    return conditional_expression_;
  }

  virtual std::ostream& print(std::ostream& os, std::string indent) const override {
    os << indent << type_ << " [" << std::endl;
    logical_or_expression_->print(os, indent + "  ");
    os << std::endl << indent << "  ? " << std::endl;
    expression_->print(os, indent + "  ");
    os << std::endl << indent << "  :" << std::endl;
    conditional_expression_->print(os, indent + "  ");
    os << std::endl << indent << "]";
    return os;
  }
};

class AssignmentExpression : public Node {
 private:
  const Node* variable_;
  std::string assignment_type_;
  const Node* rhs_;

 public:
  AssignmentExpression(const Node* variable, const std::string& assignment_type, const Node* rhs)
    : variable_(variable), assignment_type_(assignment_type), rhs_(rhs) {
    type_ = "AssignmentExpression";
  }

  const Node* getVariable() const {
    return variable_;
  }

  const std::string& getAssignmentType() const {
    return assignment_type_;
  }

  const Node* getRhs() const {
    return rhs_;
  }

  virtual std::ostream& print(std::ostream& os, std::string indent) const override {
    os << indent << type_ << " [" << std::endl;
    variable_->print(os, indent + "  ");
    os << std::endl;
    os << indent << "  type: " << assignment_type_ << std::endl;
    rhs_->print(os, indent + "  ");
    os << std::endl << indent << "]";
    return os;
  }
};


class EmptyExpression : public Node {
 private:

 public:
  EmptyExpression() {
    type_ = "EmptyExpression";
  }

  virtual std::ostream& print(std::ostream& os, std::string indent) const override {
    os << indent << type_ << " []";
    return os;
  }
};

#endif