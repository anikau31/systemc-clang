#include "SystemCClang.h"
#include "PluginAction.h"


int main(int argc, const char **argv) {
  scpar::PluginAction<SystemCClang> systemc_clang(argc, argv);
  return 0;
}
