#ifndef ast_primitives_hpp
#define ast_primitives_hpp

#include <string>
#include <iostream>

#include "ast_expression.hpp"

class Variable : public Expression {
 private:
  std::string id;
 public:
  Variable(const std::string &_id) : id(_id) {}

  const std::string getId() const {
    return id;
  }

  virtual void print(std::ostream &dst) const override
  {
    dst<<id;
  } 
};

#endif