#ifndef _TASK_DEPENDENCY_GRAPH_H_
#define _TASK_DEPENDENCY_GRAPH_H_

#include "clang/AST/DeclCXX.h"
#include "llvm/Support/raw_ostream.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "DependencyGraph.h"
#include "Model.h"
#include "FindSuspensionWindow.h"

#include <map>

namespace scpar {

  using namespace clang; 
  using namespace std;
  class TaskDependencyGraph: public RecursiveASTVisitor<TaskDependencyGraph> {

  public: 
    TaskDependencyGraph(vector<EntryFunctionContainer*> , 
		FindSuspensionWindow::waitNotifyVariablesMapType, 
		FindSuspensionWindow::waitNotifyVariablesMapType, Model::moduleMapType, llvm::raw_ostream&);

    ~TaskDependencyGraph();        

		typedef pair<string, int> nodeProcessPairType;
		typedef map<string, int> nodeProcessMapType;
		
		typedef pair<string, bool> processSuitablePairType;
		typedef map<string, bool> processSuitableMapType;

    void genDependencyGraph();	
		unsigned int getSize(const string&);
		double getProfilingTime(double);

  private:

    llvm::raw_ostream& _os;
		nodeProcessMapType _nodeProcessMap;
		vector<EntryFunctionContainer*> _ef;
		FindSuspensionWindow::waitNotifyVariablesMapType _waitVariablesMap;
		FindSuspensionWindow::waitNotifyVariablesMapType _notifyVariablesMap;
		Model::moduleMapType _moduleMap;
  };
}

#endif
