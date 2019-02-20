#include "../inc/ast.hpp"

int main() {
  std::cout << "Insert your code:" << std::endl << std::endl << "#######################"
            << std::endl;
  std::vector<const Node*> ast_roots = parseAST();
  std::cout << "#######################" << std::endl;
  for (const Node* ast : ast_roots) {
    std::cout << std::endl;
    ast->print(std::cout, "");
    std::cout << std::endl;
  }

  return 0;
}