#include "PluginAction.h"

#include "SystemCClang.h"
//#include "TimeAnalysis.h"

#include <iostream>
using namespace scpar;
using namespace std;

#include "plugins/xlat/Xlat.h"

//using namespace clang::tooling;
int main(int argc, const char **argv) {
  cout << "Plugin XLAT\n";
    PluginAction<Xlat> pa(argc, argv);
  return 0;
}
