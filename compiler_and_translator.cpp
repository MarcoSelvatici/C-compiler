#include<iostream>

#include "c_compiler/inc/compiler.hpp"
#include "c_translator/inc/translator.hpp"

int main(int argc, char** argv) {
  if (argc == 5 && std::string(argv[1]) == "-S" && std::string(argv[3]) == "-o") {
    return compile(std::string(argv[2]), std::string(argv[4]));
  }
  if (argc == 5 && std::string(argv[1]) == "--translate" && std::string(argv[3]) == "-o"){
    return translate(std::string(argv[2]), std::string(argv[4]));
  }
  std::cout << "Usage:" << std::endl
            << "-> for compiler: -S source_file -o destination_file" << std::endl
            << "-> for translator: --translate source_file -o destination_file"
            << std::endl;
  return 1;
}
