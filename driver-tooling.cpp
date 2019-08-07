#include "PluginAction.h"

#include "SystemCClang.h"
//#include "TimeAnalysis.h"

#include <iostream>
using namespace scpar;
using namespace std;

//using namespace clang::tooling;
int main(int argc, const char **argv) {
  cout << "args: " << argc << endl;
    for(int i=0;i<argc-1;i++) {
      cout << "argv[" << i << "]: " << argv[i] << endl;
    }
  PluginAction<SystemCClang> scc(argc, argv);  
  return 0;
}
