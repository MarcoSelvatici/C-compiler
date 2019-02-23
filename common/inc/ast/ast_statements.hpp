#ifndef ast_statements_hpp
#define ast_statements_hpp

#include <string>
#include <iostream>

#include "ast_node.hpp"

// This class is a node for a linked list of sequential statements.
// Example with three sequential statements.
//
//             StatementListNode
//            /                 \
//     some_statement        StatementListNode
//                          /                 \
//                   some_statement        StatementListNode
//                                        /                 \
//                                 some_statement         nullptr
class StatementListNode : public Node {
 private:
  const Node* statement_;
  const Node* next_statement_;

 public:
  StatementListNode(const Node* statement, const Node* next_statement)
    : statement_(statement), next_statement_(next_statement) {
    type_ = "StatementListNode";
  }

  const Node* getStatement() const {
    return statement_;
  }

  const Node* getNextStatement() const {
    return next_statement_;
  }

  bool hasNextStatement() const {
    return next_statement_ != nullptr;
  }

  virtual std::ostream& print(std::ostream& os, std::string indent) const override {
    os << indent << type_ << " [ " << std::endl;
    statement_->print(os, indent + "  ");
    if (hasNextStatement()) {
      os << std::endl;
      next_statement_->print(os, indent + "  ");
    }
    os << std::endl << indent << "]";
    return os;
  }
};

class ReturnStatement : public Node {
 private:
  const Node* expression_;

 public:
  ReturnStatement(const Node* expression) : expression_(expression) {
    type_ = "ReturnStatement";
  }

  const Node* getExpression() const {
    return expression_;
  }

  bool hasExpression() const {
    return expression_ != nullptr;
  }

  virtual std::ostream& print(std::ostream& os, std::string indent) const override {
    os << indent << type_ << " [ ";
    if (hasExpression()) {
      os << std::endl;
      expression_->print(os, indent + "  ");
      os << std::endl << indent;
    }
    os << "]";
    return os;
  }
};

class WhileStatement : public Node {
 private:
  const Node* condition_;
  const Node* body_;

 public:
  WhileStatement(const Node* condition, const Node* body)
    : condition_(condition), body_(body) {
    type_ = "WhileStatement";
  }

  const Node* getCondition() const {
    return condition_;
  }

  const Node* getBody() const {
    return body_;
  }

  virtual std::ostream& print(std::ostream& os, std::string indent) const override {
    os << indent << type_ << " [" << std::endl;
    condition_->print(os, indent + "  ");
    os << std::endl;
    body_->print(os, indent + "  ");
    os << std::endl << indent << "]";
    return os;
  }
};

#endif