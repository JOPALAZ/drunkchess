#include "IOhandler.h"
int main() {

  IOhandler handler(&std::cout, &std::cin);
  handler.mainLoop();
  return 0;
}