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
  DeclarationExpression(const std::string& type_specifier, const Node* variable,
                        const Node* rhs)
    : type_specifier_(type_specifier), variable_(variable), rhs_(rhs) {
    type_ = "DeclarationExpression";
  }

  const std::string& getTypeSpecifier() const {
    return type_specifier_;
  }

  const Node* getVariable() const {
    return variable_;
  }

  const Node* getRhs() const {
    return rhs_;
  }

  bool hasRhs() const {
    return rhs_ != nullptr;
  }

  virtual std::ostream& print(std::ostream& os, std::string indent) const override {
    os << indent << type_ << " [ " << type_specifier_ << std::endl;
    variable_->print(os, indent + "  ");
    if (hasRhs()) {
      os << std::endl;
      rhs_->print(os, indent + "  ");
    }
    os << std::endl << indent << "]";
    return os;
  }
};

class PostfixExpression : public Node {
 private:
  const Node* lhs_;
  std::string postfix_type_;

 public:
  // Plain declaration.
  PostfixExpression(const Node* lhs, std::string postfix_type)
    : lhs_(lhs), postfix_type_(postfix_type) {
    type_ = "PostfixExpression";
  }

  const Node* getLhs() const {
    return lhs_;
  }

  std::string getPostfixType() const {
    return postfix_type_;
  }
  
  virtual std::ostream& print(std::ostream& os, std::string indent) const override {
    os << indent << type_ << " [" << std::endl;
    os << indent << "  type: " << postfix_type_ << std::endl;
    lhs_->print(os, indent + "  ");
    os << std::endl << indent << "]";
    return os;
  }
};

class UnaryExpression : public Node {
 private:
  std::string unary_type_;
  const Node* rhs_;

 public:
  // Plain declaration.
  UnaryExpression(const std::string& unary_type, const Node* rhs)
    : unary_type_(unary_type), rhs_(rhs) {
    type_ = "UnaryExpression";
  }

  const std::string& getUnaryType() const {
    return unary_type_;
  }

  const Node* getRhs() const {
    return rhs_;
  }
  
  virtual std::ostream& print(std::ostream& os, std::string indent) const override {
    os << indent << type_ << " [" << std::endl;
    os << indent << "  type: " << unary_type_ << std::endl;
    rhs_->print(os, indent + "  ");
    os << std::endl << indent << "]";
    return os;
  }
};

class MultiplicativeExpression : public Node {
 private:
  const Node* lhs_;
  std::string multiplicative_type_;
  const Node* rhs_;

 public:
  // Plain declaration.
  MultiplicativeExpression(const Node* lhs, const std::string& multiplicative_type,
                           const Node* rhs)
    : lhs_(lhs), multiplicative_type_(multiplicative_type),rhs_(rhs) {
    type_ = "MultiplicativeExpression";
  }

  const Node* getLhs() const {
    return lhs_;
  }
  std::string getMultiplicativeType() const {
    return multiplicative_type_;
  }

  const Node* getRhs() const {
    return rhs_;
  }
  
  virtual std::ostream& print(std::ostream& os, std::string indent) const override {
    os << indent << type_ << " [" << std::endl;
    lhs_->print(os, indent + "  ");
    os << std::endl;
    os << indent << "  type: " << multiplicative_type_ << std::endl;
    rhs_->print(os, indent + "  ");
    os << std::endl << indent << "]";
    return os;
  }
};

class AdditiveExpression : public Node {
 private:
  const Node* lhs_;
  std::string additive_type_;
  const Node* rhs_;

 public:
  // Plain declaration.
  AdditiveExpression(const Node* lhs, std::string additive_type, const Node* rhs)
    : lhs_(lhs), additive_type_(additive_type), rhs_(rhs) {
    type_ = "AdditiveExpression";
  }

  const Node* getLhs() const {
    return lhs_;
  }

  const std::string& getAdditiveType() const {
    return additive_type_;
  }

  const Node* getRhs() const {
    return rhs_;
  }

  virtual std::ostream& print(std::ostream& os, std::string indent) const override {
    os << indent << type_ << " [" << std::endl;
    lhs_->print(os, indent + "  ");
    os << std::endl;
    os << indent << "  type: " << additive_type_ << std::endl;
    rhs_->print(os, indent + "  ");
    os << std::endl << indent << "]";
    return os;
  }
};

class ShiftExpression : public Node {
 private:
  const Node* lhs_;
  std::string shift_type_;
  const Node* rhs_;

 public:
  // Plain declaration.
  ShiftExpression(const Node* lhs, std::string shift_type, const Node* rhs)
    : lhs_(lhs), shift_type_(shift_type), rhs_(rhs) {
    type_ = "ShiftExpression";
  }

  const Node* getLhs() const {
    return lhs_;
  }

  const std::string& getShiftType() const {
    return shift_type_;
  }

  const Node* getRhs() const {
    return rhs_;
  }

  virtual std::ostream& print(std::ostream& os, std::string indent) const override {
    os << indent << type_ << " [" << std::endl;
    lhs_->print(os, indent + "  ");
    os << std::endl;
    os << indent << "  type: " << shift_type_ << std::endl;
    rhs_->print(os, indent + "  ");
    os << std::endl << indent << "]";
    return os;
  }
};

class RelationalExpression : public Node {
 private:
  const Node* lhs_;
  std::string relational_type_;
  const Node* rhs_;

 public:
  // Plain declaration.
  RelationalExpression(const Node* lhs, std::string relational_type, const Node* rhs)
    : lhs_(lhs), relational_type_(relational_type), rhs_(rhs) {
    type_ = "RelationalExpression";
  }

  const Node* getLhs() const {
    return lhs_;
  }

  const std::string& getRelationalType() const {
    return relational_type_;
  }

  const Node* getRhs() const {
    return rhs_;
  }

  virtual std::ostream& print(std::ostream& os, std::string indent) const override {
    os << indent << type_ << " [" << std::endl;
    lhs_->print(os, indent + "  ");
    os << std::endl;
    os << indent << "  type: " << relational_type_ << std::endl;
    rhs_->print(os, indent + "  ");
    os << std::endl << indent << "]";
    return os;
  }
};

class EqualityExpression : public Node {
 private:
  const Node* lhs_;
  std::string equality_type_;
  const Node* rhs_;

 public:
  // Plain declaration.
  EqualityExpression(const Node* lhs, std::string equality_type, const Node* rhs)
    : lhs_(lhs), equality_type_(equality_type), rhs_(rhs) {
    type_ = "EqualityExpression";
  }

  const Node* getLhs() const {
    return lhs_;
  }

  const std::string& getEqualityType() const {
    return equality_type_;
  }

  const Node* getRhs() const {
    return rhs_;
  }

  virtual std::ostream& print(std::ostream& os, std::string indent) const override {
    os << indent << type_ << " [" << std::endl;
    lhs_->print(os, indent + "  ");
    os << std::endl;
    os << indent << "  type: " << equality_type_ << std::endl;
    rhs_->print(os, indent + "  ");
    os << std::endl << indent << "]";
    return os;
  }
};

class AndExpression : public Node {
 private:
  const Node* lhs_;
  const Node* rhs_;

 public:
  // Plain declaration.
  AndExpression(const Node* lhs, const Node* rhs) : lhs_(lhs), rhs_(rhs) {
    type_ = "AndExpression";
  }

  const Node* getLhs() const {
    return lhs_;
  }

  const Node* getRhs() const {
    return rhs_;
  }

  virtual std::ostream& print(std::ostream& os, std::string indent) const override {
    os << indent << type_ << " [" << std::endl;
    lhs_->print(os, indent + "  ");
    os << std::endl;
    os << indent << "  type: " << " & "  << std::endl;
    rhs_->print(os, indent + "  ");
    os << std::endl << indent << "]";
    return os;
  }
};

class ExclusiveOrExpression : public Node {
 private:
  const Node* lhs_;
  const Node* rhs_;

 public:
  // Plain declaration.
  ExclusiveOrExpression(const Node* lhs, const Node* rhs) : lhs_(lhs), rhs_(rhs) {
    type_ = "ExclusiveOrExpression";
  }

  const Node* getLhs() const {
    return lhs_;
  }

  const Node* getRhs() const {
    return rhs_;
  }

  virtual std::ostream& print(std::ostream& os, std::string indent) const override {
    os << indent << type_ << " [" << std::endl;
    lhs_->print(os, indent + "  ");
    os << std::endl;
    os << indent << "  type: " << " ^ "  << std::endl;
    rhs_->print(os, indent + "  ");
    os << std::endl << indent << "]";
    return os;
  }
};

class InclusiveOrExpression : public Node {
 private:
  const Node* lhs_;
  const Node* rhs_;

 public:
  // Plain declaration.
  InclusiveOrExpression(const Node* lhs, const Node* rhs) : lhs_(lhs), rhs_(rhs) {
    type_ = "InclusiveOrExpression";
  }

  const Node* getRhs() const {
    return rhs_;
  }

  const Node* getLhs() const {
    return lhs_;
  }

  virtual std::ostream& print(std::ostream& os, std::string indent) const override {
    os << indent << type_ << " [" << std::endl;
    lhs_->print(os, indent + "  ");
    os << std::endl;
    os << indent << "  type: " << " | "  << std::endl;
    rhs_->print(os, indent + "  ");
    os << std::endl << indent << "]";
    return os;
  }
};

class LogicalAndExpression : public Node {
 private:
  const Node* lhs_;
  const Node* rhs_;

 public:
  // Plain declaration.
  LogicalAndExpression(const Node* lhs, const Node* rhs) : lhs_(lhs), rhs_(rhs) {
    type_ = "LogicalAndExpression";
  }

  const Node* getLhs() const {
    return lhs_;
  }

  const Node* getRhs() const {
    return rhs_;
  }

  virtual std::ostream& print(std::ostream& os, std::string indent) const override {
    os << indent << type_ << " [" << std::endl;
    lhs_->print(os, indent + "  ");
    os << std::endl;
    os << indent << "  type: " << " && "  << std::endl;
    rhs_->print(os, indent + "  ");
    os << std::endl << indent << "]";
    return os;
  }
};

class LogicalOrExpression : public Node {
 private:
  const Node* lhs_;
  const Node* rhs_;

 public:
  // Plain declaration.
  LogicalOrExpression(const Node* lhs, const Node* rhs) : lhs_(lhs), rhs_(rhs) {
    type_ = "LogicalOrExpression";
  }

  const Node* getLhs() const {
    return lhs_;
  }

  const Node* getRhs() const {
    return rhs_;
  }

  virtual std::ostream& print(std::ostream& os, std::string indent) const override {
    os << indent << type_ << " [" << std::endl;
    lhs_->print(os, indent + "  ");
    os << std::endl;
    os << indent << "  type: " << " || "  << std::endl;
    rhs_->print(os, indent + "  ");
    os << std::endl << indent << "]";
    return os;
  }
};

class ConditionalExpression : public Node {
 private:
  const Node* condition_;
  const Node* expression1_;
  const Node* expression2_;

 public:
  // Plain declaration.
  ConditionalExpression(const Node* condition, const Node* expression1, const Node* expression2)
    : condition_(condition), expression1_(expression1), expression2_(expression2) {
    type_ = "ConditionalExpression";
  }

  const Node* getCondition() const {
    return condition_;
  }

  const Node* getExpression1() const {
    return expression1_;
  }

  const Node* getExpression2() const {
    return expression2_;
  }

  virtual std::ostream& print(std::ostream& os, std::string indent) const override {
    os << indent << type_ << " [" << std::endl;
    condition_->print(os, indent + "  ");
    os << std::endl << indent << "  ? " << std::endl;
    expression1_->print(os, indent + "  ");
    os << std::endl << indent << "  :" << std::endl;
    expression2_->print(os, indent + "  ");
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