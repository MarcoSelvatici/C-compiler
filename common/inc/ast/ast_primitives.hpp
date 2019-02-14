#ifndef ast_primitives_hpp
#define ast_primitives_hpp

#include <string>
#include <iostream>

#include "ast_expression.hpp"

class Identifier : public Expression {
 private:
  std::string id_;
 public:
  Identifier(const std::string& id) : id_(id) {}

  const std::string getId() const {
    return id_;
  }

  virtual void print(std::ostream& os) const override
  {
    os << id_;
  } 
};

#endif