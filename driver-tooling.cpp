#include "PluginAction.h"

//#include "SystemCClang.h"
#include "TimeAnalysis.h"

using namespace scpar;
//using namespace clang::tooling;
int main(int argc, const char **argv)
{
  
		PluginAction<TimeAnalysis> scc(argc, argv);  
  return 0;
}
