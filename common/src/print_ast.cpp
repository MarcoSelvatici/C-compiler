#include "../inc/ast.hpp"

int main() {
  std::cout << "Insert your code in one line:" << std::endl;
  std::vector<const Node*> ast_roots = parseAST();
  for (const Node* ast : ast_roots) {
    ast->print(std::cerr, "");
    std::cerr << std::endl;
  }

  return 0;
}