// Virtual base class for the nodes of the ast.
// Every node in the ast will have a type to identify it.
// In addition to the type, there may be other structures (defined in the subclasses
// themselves) like the id, or pointers to other nodes of the ast.

#ifndef ast_expression_hpp
#define ast_expression_hpp

#include <string>
#include <iostream>
#include <map>

#include <memory>

class Expression;

typedef const Expression *ExpressionPtr;

class Expression {
 protected:
  std::string type_;

  virtual const std::string& getType() const {
    return type_;
  };

 public:
  virtual ~Expression() {}

  // Tell and expression to print itself to the given stream.
  virtual void print(std::ostream& dst) const = 0;
};


#endif
