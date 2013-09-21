#include "PluginAction.h"

//#include "SystemCClang.h"

using namespace scpar;
//using namespace clang::tooling;

#include "SCuitable.h"
#include "TimeAnalysis.h"
int main(int argc, const char **argv)
{
  
//		PluginAction<SystemCConsumer> scc(argc, argv);
//	  PluginAction<SCuitable> optimize(argc, argv);  
		PluginAction<TimeAnalysis> ta(argc, argv);
  return 0;
}
