#ifndef ast_enums_hpp
#define ast_enums_hpp

#include <string>
#include <iostream>

#include "ast_node.hpp"

// List of elements in an enum. The structure of this list is based on the same idea
// as the one for StatementListNode.
class EnumDeclarationListNode : public Node {
 private:
  const Node* enum_declaration_;
  const Node* next_enum_declaration_;

 public:
  EnumDeclarationListNode(const Node* enum_declaration, const Node* next_enum_declaration)
    : enum_declaration_(enum_declaration), next_enum_declaration_(next_enum_declaration) {
    type_ = "EnumDeclarationListNode";
  }

  const Node* getEnumDeclaration() const {
    return enum_declaration_;
  }

  const Node* getNextEnumDeclaration() const {
    return next_enum_declaration_;
  }

  bool hasNextEnumDeclaration() const {
    return next_enum_declaration_ != nullptr;
  }

  virtual std::ostream& print(std::ostream& os, std::string indent) const override {
    os << indent << type_ << " [" << std::endl;
    enum_declaration_->print(os, indent + "  ");
    if (hasNextEnumDeclaration()) {
      os << std::endl;
      next_enum_declaration_->print(os, indent + "  ");
    }
    os << std::endl << indent << "]";
    return os;
  }
};

class EnumDeclaration : public Node {
 private:
  std::string id_;
  const Node* initialization_value_;

 public:
  EnumDeclaration(const std::string& id, const Node* initialization_value)
    : id_(id), initialization_value_(initialization_value) {
    type_ = "EnumDeclaration";
  }

  const std::string& getId() const {
    return id_;
  }

  bool hasInitializationValue() const {
    return initialization_value_ != nullptr;
  }

  const Node* getInitializationValue() const {
    return initialization_value_;
  }

  virtual std::ostream& print(std::ostream& os, std::string indent) const override {
    os << indent << type_ << " [ " << id_;
    if (hasInitializationValue()) {
      os << std::endl;
      initialization_value_->print(os, indent + "  ");
    }
    os << std::endl << indent << "]";
    return os;
  }
};

#endif
