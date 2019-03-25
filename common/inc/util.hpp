#ifndef util_hpp
#define util_hpp

#include <iostream>

class Util {
 public:
  static const bool DEBUG = false;

  static void abort() {
    if (DEBUG) {
      std::cerr << std::endl << "Aborting." << std::endl;
    }
    std::exit(-1);
  }
};

#endif
