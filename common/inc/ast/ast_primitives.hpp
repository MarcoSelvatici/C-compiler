#ifndef ast_primitives_hpp
#define ast_primitives_hpp

#include <string>
#include <iostream>

#include "ast_node.hpp"

class Variable : public Node {
 private:
  std::string id_;
  // Specifies if
  // - normal var: "normal",
  // - array: "array",
  // - etc...
  std::string info_;
  const Node* array_index_or_size_;

 public:
  Variable(const std::string& id, const std::string& info,
           const Node* array_index_or_size)
    : id_(id), info_(info), array_index_or_size_(array_index_or_size) {
    type_ = "Variable";
  }

  const std::string& getId() const {
    return id_;
  }

  const std::string& getInfo() const {
    return info_;
  }

  bool hasArrayIndexOrSize() const {
    return (info_ == "array" && array_index_or_size_ != nullptr);
  }

  const Node* getArrayIndexOrSize() const {
    if (!hasArrayIndexOrSize()) {
      if (Util::DEBUG) {
        std::cerr << "Trying to get array index or size for " << id_ << " that is not an "
                  << "an array." << std::endl;
      }
      Util::abort();
    }
    return array_index_or_size_;
  }

  virtual std::ostream& print(std::ostream& os, std::string indent) const override {
    os << indent << type_ << " [ " << id_ << " (" << info_ << ")";
    if (info_ == "array" && hasArrayIndexOrSize()) {
      os << std::endl << indent << "index or size:" << std::endl;
      array_index_or_size_->print(os, indent + "  ");
    }
    os << std::endl << indent << "]";
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