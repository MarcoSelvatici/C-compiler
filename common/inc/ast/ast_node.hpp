// Virtual base class for the nodes of the ast.
// Every node in the ast will have a type to identify it.
// In addition to the type, there may be other structures (defined in the subclasses
// themselves) like the id, or pointers to other nodes of the ast.

#ifndef ast_node_hpp
#define ast_node_hpp

#include "../util.hpp"

#include <string>
#include <iostream>
#include <map>

#include <memory>

class Node;

typedef const Node* NodePtr;

class Node {
 protected:
  std::string type_;

 public:
  virtual ~Node() {}

  virtual const std::string& getType() const {
    return type_;
  };

  // Tell and node to print itself to the given stream.
  virtual std::ostream& print(std::ostream& dst, std::string indent) const = 0;
};


#endif
