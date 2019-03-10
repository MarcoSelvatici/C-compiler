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

  bool hasStatement() const {
    return statement_ != nullptr;
  }

  bool hasNextStatement() const {
    return next_statement_ != nullptr;
  }

  // By how the parser creates the AST, iff the are no statements at all hasStatement will
  // be false.
  bool isEmptyStatementList() const {
    return !hasStatement();
  }

  virtual std::ostream& print(std::ostream& os, std::string indent) const override {
    os << indent << type_ << " [";
    if (hasStatement()) {
      os << std::endl;
      statement_->print(os, indent + "  ");
    }
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

class BreakStatement : public Node {
 
 public:
  BreakStatement(){
    type_ = "BreakStatement";
  }

  virtual std::ostream& print(std::ostream& os, std::string indent) const override {
    os << indent << type_;
    return os;
  }
};

class ContinueStatement : public Node {
 
 public:
  ContinueStatement(){
    type_ = "ContinueStatement";
  }

  virtual std::ostream& print(std::ostream& os, std::string indent) const override {
    os << indent << type_;
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

class IfStatement : public Node {
 private:
  const Node* condition_;
  const Node* if_body_;
  const Node* else_body_;

 public:
  IfStatement(const Node* condition, const Node* if_body, const Node* else_body)
    : condition_(condition), if_body_(if_body), else_body_(else_body) {
    type_ = "IfStatement";
  }

  const Node* getCondition() const {
    return condition_;
  }

  const Node* getIfBody() const {
    return if_body_;
  }

  const Node* getElseBody() const {
    return else_body_;
  }

  bool hasElseBody() const {
    return else_body_ != nullptr;
  }

  virtual std::ostream& print(std::ostream& os, std::string indent) const override {
    os << indent << type_ << " [" << std::endl;
    condition_->print(os, indent + "  ");
    os << std::endl;
    if_body_->print(os, indent + "  ");
    if (hasElseBody()) {
      os << std::endl;
      else_body_->print(os, indent + "  ");
    }
    os << std::endl << indent << "]";
    return os;
  }
};

#endif