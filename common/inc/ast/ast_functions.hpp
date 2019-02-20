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

class ArgumentsList : public Node {
 private:
  // Empty for now, only empty arguments lists are implemented.
 public:
  ArgumentsList() {
    type_ = "ArgumentsList";
  }

  virtual std::ostream& print(std::ostream& os, std::string indent) const override {
    os << indent << type_ << " [] ";
    return os;
  }
};

#endif