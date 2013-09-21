#include "TaskDependencyGraph.h"
using namespace scpar;

TaskDependencyGraph::TaskDependencyGraph (vector<EntryFunctionContainer*> ef, 
FindSuspensionWindow::waitNotifyVariablesMapType waitVariableMap, 
FindSuspensionWindow::waitNotifyVariablesMapType notifyVariableMap, 
Model::moduleMapType moduleMap, llvm::raw_ostream & os):
_os (os),
_ef(ef),
_waitVariablesMap(waitVariableMap),
_notifyVariablesMap(notifyVariableMap),
_moduleMap(moduleMap)
{

}

TaskDependencyGraph::~TaskDependencyGraph ()
{
	_nodeProcessMap.clear();
}

double TaskDependencyGraph::getProfilingTime(double bytes) {

	double totalTime = 0;
  double bytesMB = bytes/1048576;

  if(bytesMB < 0.01) {
    totalTime = totalTime + 0.01;
  }
	else if (bytesMB <= 1) {
		totalTime = totalTime + 0.1;
	}
  else {
    totalTime = totalTime + bytesMB*0.4;
  }
  return totalTime;
}

void TaskDependencyGraph::genDependencyGraph ()
{

  TaskGraph g;
  int nodeCounter = 2;
  int duplicateEdge = 0;
  pair < string, string > tmpPair;
  Task *tc, *tg;
  tc = g.addTask (0);		//cpu node
  tg = g.addTask (1);		//gpu node


  for (int i = 0 ; i<_ef.size(); i++) {
		EntryFunctionContainer *cef = _ef.at(i);
		Task *t = g.addTask(nodeCounter);
		_nodeProcessMap.insert(nodeProcessPairType(cef->getName(), nodeCounter));
		nodeCounter++;
		
		FindWait::waitTimesMapType waitTimesMap = cef->getWaitTimesMap();
		FindWait::dataTimesMapType dataTimesMap = cef->getDataTimesMap();
		FindWait::waitNestLoopMapType waitNestLoopMap = cef->getWaitNestLoopMap();
		FindWait::dataNestLoopMapType dataNestLoopMap = cef->getDataNestLoopMap();
		FindLoopTime::forStmtLoopTimeMapType loopTimeMap = cef->getLoopTimeMap();	
		// Finding which module decl the current entry function belongs to. Should 
		// have this information in entryFunction class to avoid the code below...
		ModuleDecl* currModuleDecl;
		for (Model::moduleMapType::iterator it = _moduleMap.begin() , eit = 
		_moduleMap.end();
		it != eit;
		it++) {
			ModuleDecl* curr = it->second;
			ModuleDecl::processMapType currProcessMap = curr->getProcessMap();			
			if(currProcessMap.find(cef->getName()) != currProcessMap.end() ) {
				currModuleDecl = curr;
			}
		}

		double time = 0;
		ModuleDecl::dataMemberMapType currDataMemberMap = currModuleDecl->getDataMemberMap();

		for (FindSuspensionWindow::waitNotifyVariablesMapType::iterator it = 
		_waitVariablesMap.begin(), eit = _waitVariablesMap.end();
		it != eit;
		it++) {
			vector<VariableAccess*> tmp = it->second;
			if(waitTimesMap.find(it->first) != waitTimesMap.end()) {
				// have to compare nest levels 
				FindWait::waitTimesMapType::iterator waitTimesFound = 
				waitTimesMap.find(it->first);
				if(waitNestLoopMap.find(it->first) != waitNestLoopMap.end()) {
					FindWait::waitNestLoopMapType::iterator waitNestLoopFound = 
					waitNestLoopMap.find(it->first);
					for(int j = 0; j<tmp.size(); j++) {	
						if(currDataMemberMap.find(tmp.at(j)->returnVariableName()) != 
						currDataMemberMap.end()) {
							ModuleDecl::dataMemberMapType::iterator dataFound = 
							currDataMemberMap.find(tmp.at(j)->returnVariableName());							
							time = time + 
							getProfilingTime(dataFound->second->getSize())*2*waitTimesFound->second;										 							
						}
					}
				}
			}
		}
		Connection *c2 = g.addConnection(tc, t);
		Connection *c1 = g.addConnection(t, tg);				
		double cpuTime = 0;
		double gpuTime = 0;
		for (FindLoopTime::forStmtLoopTimeMapType::iterator it = loopTimeMap.begin(), 
		eit = loopTimeMap.end();
		it != eit;
		it++) {
			cpuTime = cpuTime + it->second.first;
			gpuTime = gpuTime + it->second.second;
		}
		c2->addDataTransferOverhead(time);
		c1->addDataTransferOverhead(0);
		c2->addKernelTime(gpuTime);
		c1->addKernelTime(cpuTime);
	}

	for(nodeProcessMapType::iterator it = _nodeProcessMap.begin(), 
	eit = _nodeProcessMap.end();
	it != eit;
	it++) {
		_os <<"\n Process Name : " <<it->first<< " id : " <<it->second;
	}
		g.dump(_os , 0);		
		g.Optimization(_os);
}

