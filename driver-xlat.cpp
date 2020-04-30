#include "plugins/xlat/Xlat.h"
//#include "PluginAction.h"

#include "plugins/xlat/Xlat.h"

int main(int argc, const char **argv) {
  llvm::outs() << "Xlat plugin \n";
    XlatPluginAction pa(argc, argv);
  return 0;
}
