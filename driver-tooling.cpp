#include "SystemCClang.h"
#include "PluginAction.h"
using namespace scpar;
using namespace std;


int main(int argc, const char **argv) {
  PluginAction<SystemCClang> systemc_clang(argc, argv);
  return 0;
}
