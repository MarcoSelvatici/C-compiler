#include "../inc/compiler.hpp"

#include <fstream>

int compile(const std::string& source_file_name,
            const std::string& destination_file_name) {
  FILE* file_in;
  if (!(file_in = fopen(source_file_name.c_str(), "r"))) {
    std::cerr << "Cannot open source file: '" << source_file_name << "'." << std::endl;
    return 1;
  }
  // Set file Flex and Yacc will read from.
  yyset_in(file_in);

  // Prepare asm output file.
  std::ofstream asm_out;
  asm_out.open(destination_file_name);

  // Implement compiler.
  const Expression* ast = parseAST();
  ast->print(asm_out);

  // Close the files.
  fclose(file_in);
  asm_out.close();
  return 0;
}