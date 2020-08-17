#include "plugins/hdl/HDLMain.h"
//#include "PluginAction.h"

int main(int argc, const char **argv) {
  llvm::outs() << "HDL plugin \n";
    HDLPluginAction pa(argc, argv);
  return 0;
}
