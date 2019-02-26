#ifndef ast_functions_hpp
#define ast_functions_hpp

#include <string>
#include <iostream>

#include "ast_node.hpp"

class FunctionDefinition : public Node {
 private:
  std::string type_specifier_;
  const Node* name_;
  const Node* argument_list_;
  const Node* body_;

 public:
  FunctionDefinition(const std::string& type_specifier, const Node* name,
                     const Node* argument_list, const Node* body)
    : type_specifier_(type_specifier), name_(name), argument_list_(argument_list),
      body_(body) {
    type_ = "FunctionDefinition";
  }

  const std::string& getTypeSpecifier() const {
    return type_specifier_;
  }

  const Node* getName() const {
    return name_;
  }

  const Node* getArgumentList() const {
    return argument_list_;
  }

  const Node* getBody() const {
    return body_;
  }

  virtual std::ostream& print(std::ostream& os, std::string indent) const override {
    os << indent << type_ << " [ " << type_specifier_ << std::endl;
    name_->print(os, indent + "  ");
    os << std::endl;
    argument_list_->print(os, indent + "  ");
    os << std::endl;
    body_->print(os, indent + "  ");
    os << std::endl << indent << "]";
    return os;
  }
};

// List of arguments of a function. The structure of this list is based on the same idea
// as the one for StatementListNode.
class ArgumentListNode : public Node {
 private:
  const Node* argument_;
  const Node* next_argument_;

 public:
  ArgumentListNode(const Node* argument, const Node* next_argument)
    : argument_(argument), next_argument_(next_argument) {
    type_ = "ArgumentListNode";
  }

  const Node* getArgument() const {
    return argument_;
  }

  const Node* getNextArgument() const {
    return next_argument_;
  }

  bool hasArgument() const {
    return argument_ != nullptr;
  }

  bool hasNextArgument() const {
    return next_argument_ != nullptr;
  }

  // By how the parser creates the AST, iff the are no arguments at all hasArgument will
  // be false.
  bool isEmptyArgumentList() const {
    return hasArgument();
  }

  virtual std::ostream& print(std::ostream& os, std::string indent) const override {
    os << indent << type_ << " [";
    if (hasArgument()) {
      os << std::endl;
      argument_->print(os, indent + "  ");
    }
    if (hasNextArgument()) {
      os << std::endl;
      next_argument_->print(os, indent + "  ");
    }
    os << std::endl << indent << "]";
    return os;
  }
};

#endif