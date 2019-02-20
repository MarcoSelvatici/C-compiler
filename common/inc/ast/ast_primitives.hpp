#ifndef ast_primitives_hpp
#define ast_primitives_hpp

#include <string>
#include <iostream>

#include "ast_node.hpp"

class Variable : public Node {
 private:
  std::string id_;
  // Specifies if normal var, array, pointer etc...
  std::string info_;

 public:
  Variable(const std::string& id, const std::string& info) : id_(id), info_(info) {
    type_ = "Variable";
  }

  const std::string& getId() const {
    return id_;
  }

  const std::string& getInfo() const {
    return info_;
  }

  virtual std::ostream& print(std::ostream& os, std::string indent) const override {
    os << indent << type_ << " [ " << id_ << " (" << info_ << ")" << " ]";
    return os;
  }
};

class IntegerConstant : public Node {
 private:
  long long int value_;

 public:
  IntegerConstant(long long int value) : value_(value) {
    type_ = "IntegerConstant";
  }

  long long int getValue() const {
    return value_;
  }

  virtual std::ostream& print(std::ostream& os, std::string indent) const override {
    os << indent << type_ << " [ " << value_ << " ]";
    return os;
  }
};

#endif