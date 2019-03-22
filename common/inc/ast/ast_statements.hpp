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

class CompoundStatement : public Node {
 private:
  const Node* statement_list_;

 public:
  CompoundStatement(const Node* statement_list) : statement_list_(statement_list) {
    type_ = "CompoundStatement";
  }

  const Node* getStatementList() const {
    return statement_list_;
  }

  bool hasStatementList() const {
    return statement_list_ != nullptr;
  }

  virtual std::ostream& print(std::ostream& os, std::string indent) const override {
    os << indent << type_ << " [ ";
    if (hasStatementList()) {
      os << std::endl;
      statement_list_->print(os, indent + "  ");
      os << std::endl << indent;
    }
    os << "]";
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

class ForStatement : public Node {
 private:
  const Node* init_;
  const Node* condition_;
  const Node* increment_;
  const Node* body_;

 public:
  ForStatement(const Node* init, const Node* condition, const Node* increment,
                 const Node* body)
    :  init_(init), condition_(condition), increment_(increment), body_(body) {
    type_ = "ForStatement";
  }

  const Node* getInit() const {
    return init_;
  }

  const Node* getCondition() const {
    return condition_;
  }

  const Node* getIncrement() const {
    return increment_;
  }
 
  const Node* getBody() const {
    return body_;
  }

  bool hasIncrement() const {
    return !(increment_ == nullptr);
  }

  virtual std::ostream& print(std::ostream& os, std::string indent) const override {
    os << indent << type_ << " [" << std::endl;
    init_->print(os, indent + "  ");
    os << std::endl;
    condition_->print(os, indent + "  ");
    os << std::endl;
    if (hasIncrement()){
      increment_->print(os, indent + "  ");
      os << std::endl;
    }
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

class SwitchStatement : public Node {
 private:
  const Node* test_;
  const Node* body_;

 public:
  SwitchStatement(const Node* test, const Node* body)
    : test_(test), body_(body) {
    type_ = "SwitchStatement";
  }

  const Node* getTest() const {
    return test_;
  }

  const Node* getBody() const {
    return body_;
  }

  bool hasBody() const {
    return body_ != nullptr;
  }

  virtual std::ostream& print(std::ostream& os, std::string indent) const override {
    os << indent << type_ << " [" << std::endl;
    test_->print(os, indent + "  ");
    os << std::endl;
    if (hasBody()) {
      os << std::endl;
      body_->print(os, indent + "  ");
    }
    os << std::endl << indent << "]";
    return os;
  }
};

class CaseStatementListNode : public Node {
 private:
  const Node* case_statement_;
  const Node* next_case_statement_;

 public:
  CaseStatementListNode(const Node* case_statement, const Node* next_case_statement)
    : case_statement_(case_statement), next_case_statement_(next_case_statement) {
    type_ = "CaseStatementListNode";
  }

  const Node* getCaseStatement() const {
    return case_statement_;
  }

  const Node* getNextCaseStatement() const {
    return next_case_statement_;
  }

  bool hasCaseStatement() const {
    return case_statement_ != nullptr;
  }

  bool hasNextCaseStatement() const {
    return next_case_statement_ != nullptr;
  }

  // By how the parser creates the AST, iff the are no statements at all hasStatement will
  // be false.
  bool isEmptyCaseStatementList() const {
    return !hasCaseStatement();
  }

  virtual std::ostream& print(std::ostream& os, std::string indent) const override {
    os << indent << type_ << " [";
    if (hasCaseStatement()) {
      os << std::endl;
      case_statement_->print(os, indent + "  ");
    }
    if (hasNextCaseStatement()) {
      os << std::endl;
      next_case_statement_->print(os, indent + "  ");
    }
    os << std::endl << indent << "]";
    return os;
  }
};


class CaseStatement : public Node {
 private:
  const Node* case_expr_;
  const Node* body_;

 public:
  CaseStatement(const Node* case_expr, const Node* body)
    : case_expr_(case_expr), body_(body) {
    type_ = "CaseStatement";
  }

  const Node* getCaseExpr() const {
    return case_expr_;
  }

  const Node* getBody() const {
    return body_;
  }

  bool hasBody() const {
    return !(body_ == nullptr);
  }

  virtual std::ostream& print(std::ostream& os, std::string indent) const override {
    os << indent << type_ << " [" << std::endl;
    case_expr_->print(os, indent + "  ");
    os << std::endl;
    if (hasBody()){
      body_->print(os, indent + "  ");
      os << std::endl << indent << "]";
    }
    return os;
  }
};

class DefaultStatement : public Node {
 private:
  const Node* body_;

 public:
  DefaultStatement(const Node* body)
    : body_(body) {
    type_ = "DefaultStatement";
  }

  const Node* getBody() const {
    return body_;
  }
  
  bool hasBody() const {
    return !(body_ == nullptr);
  }


  virtual std::ostream& print(std::ostream& os, std::string indent) const override {
    os << indent << type_ << " [" << std::endl;
    if (hasBody()){
      body_->print(os, indent + "  ");
      os << std::endl << indent << "]";
    }
    return os;
  }
};


#endif