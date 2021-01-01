#include "plugins/hdl/HDLMain.h"
//#include "PluginAction.h"


int main(int argc, const char **argv) {
  llvm::outs() << "HDL plugin \n";
  systemc_hdl::HDLPluginAction pa(argc, argv);
  return 0;
}
