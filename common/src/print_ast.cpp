#include "../inc/ast.hpp"

int main() {
  std::cout << "Insert your code in one line:" << std::endl;
  const Expression* ast = parseAST();

  ast->print(std::cout);

  return 0;
}