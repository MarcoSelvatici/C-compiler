#include "../inc/translator.hpp"

#include "../../common/inc/ast.hpp"
#include "../../common/inc/util.hpp"

#include <fstream>

// Indentaion step for the translated code.
const std::string IS = "  ";

// il is indentation level.
void indent(std::ofstream& py_out, int il) {
  for (int i = 0; i < il; i++) {
    py_out << IS;
  }
}

void translateVariableDeclaration(std::ofstream& py_out,
                                  const DeclarationExpression* declaration_expression,
                                  int il) {
  if (Util::DEBUG) {
    std::cerr << "==> Translating variable declaration." << std::endl;
  }

  // Extract id.
  const Variable* variable =
      dynamic_cast<const Variable*>(declaration_expression->getVariable());
  const std::string& variable_id = variable->getId();
  // Evaluate rhs.
  if (declaration_expression->hasRhs()) {
    // TODO: implement arithmetic expressions.
    indent(py_out, il);
    py_out << variable_id << " = 0" << std::endl;
  } else {
    indent(py_out, il);
    py_out << variable_id << " = 0" << std::endl;
  }
}

// Translates root level, i.e. global scope. Includes:
// - definition of global integer variables.
// - definition of functions.
void translateRootLevel(std::ofstream& py_out, const Node* ast) {
  if (Util::DEBUG) {
    std::cerr << "==> Translating root level." << std::endl;
  }

  // Global variable declaration.
  if (ast->getType() == "DeclarationExpression") {
    const DeclarationExpression* declaration_expression =
      dynamic_cast<const DeclarationExpression*>(ast);
    if (declaration_expression->getTypeSpecifier() != "int") {
      if (Util::DEBUG) {
        std::cerr << "Unexpected variable with non-int type: "
                  << declaration_expression->getTypeSpecifier() << "." << std::endl;
      }
      Util::abort();
    }
    translateVariableDeclaration(py_out, declaration_expression, 0);
  }
  // Function definition.
  else if (ast->getType() == "FunctionDefinition") {
    // TODO.
  }
  // Unkonwn or unexpected node.
  else {
    if (Util::DEBUG) {
      std::cerr << "WARNING: Unkown or unexpected node type: " << ast->getType()
                << std::endl;
    }
  }
}

void translateAST(std::vector<const Node*> ast_roots, std::ofstream& py_out ) {
  for (const Node* ast : ast_roots) {
    if(Util::DEBUG) {
      std::cerr << std::endl
                << "============ AST ============" << std::endl;
      ast->print(std::cerr, "");
      std::cerr << std::endl << std::endl
                << "======== TRANSLATION ========" << std::endl;
    }
    translateRootLevel(py_out, ast);
  }

  // Invoke main as the starting point
  py_out << std::endl;
  py_out << "if __name__ == '__main__':" << std::endl
         << IS << "import sys" << std::endl
         << IS << "ret = main()" << std::endl
         << IS << "sys.exit(ret)" << std::endl;
}

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
  translateAST(ast_roots, py_out);

  // Close the files.
  fclose(file_in);
  py_out.close();
  return 0;
}
