#include "PluginAction.h"

#include "SystemCClang.h"
//#include "TimeAnalysis.h"

#include <iostream>
using namespace scpar;
using namespace std;

//using namespace clang::tooling;
int main(int argc, const char **argv) {
  PluginAction<SystemCClang> scc(argc, argv);  
  return 0;
}
