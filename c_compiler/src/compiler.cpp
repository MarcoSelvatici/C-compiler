#include "../inc/compiler.hpp"

#include <fstream>

int compile(const std::string& source_file, const std::string& destination_file) {
  FILE* file_in;
  if (!(file_in = fopen(source_file.c_str(), "r"))) {
    std::cerr << "Cannot open source file: '" << source_file << "'." << std::endl;
    return 1;
  }
  yyset_in(file_in);
  
  const Expression* ast = parseAST();
  ast->print(std::cerr);

  fclose(file_in);
  return 0;
}