#include "../inc/translator.hpp"

#include "../../common/inc/ast.hpp"
#include "../../common/inc/util.hpp"

#include <fstream>

int translate(const std::string& source_file_name,
              const std::string& destination_file_name) {
  FILE* file_in;
  if (!(file_in = fopen(source_file_name.c_str(), "r"))) {
    std::cerr << "Cannot open source file: '" << source_file_name << "'." << std::endl;
    return 1;
  }
  // Set file Flex and Yacc will read from.
  yyset_in(file_in);

  // Prepare python output file.
  std::ofstream py_out;
  py_out.open(destination_file_name);

  // Implement translator.
  std::vector<const Node*> ast_roots = parseAST();
  for (const Node* ast : ast_roots) {
    if(Util::DEBUG) {
      ast->print(std::cerr, "");
      std::cerr << std::endl;
    }
  }

  // Close the files.
  fclose(file_in);
  py_out.close();
  return 0;
}
