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