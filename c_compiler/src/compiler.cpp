#include "../inc/compiler.hpp"

#include "../../common/inc/ast.hpp"
#include "../../common/inc/util.hpp"

#include <fstream>

unsigned int unique_id_counter = 0;

std::string makeUniqueId(const std::string& base_id) {
  return "_" + base_id + "_" + std::to_string(unique_id_counter++);
}

void compileAst(const Expression* ast, std::ofstream& asm_out) {
  // TODO.
}

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
  std::vector<const Expression*> ast_roots = parseAST();
  for (const Expression* ast : ast_roots) {
    if(Util::DEBUG) {
      ast->print(std::cerr);
      std::cerr << std::endl;
    }
    compileAst(ast, asm_out);
  }

  // Close the files.
  fclose(file_in);
  asm_out.close();
  return 0;
}