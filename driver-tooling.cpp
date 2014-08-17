#include "PluginAction.h"

//#include "SystemCClang.h"

using namespace scpar;
//using namespace clang::tooling;

#include "TimeAnalysis.h"
int main(int argc, const char **argv)
{
  
		PluginAction<SystemCConsumer> scc(argc, argv);  
  return 0;
}
