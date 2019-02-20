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

#endif