#include "FindSuspensionTime.h"
#include "FindArgument.h"
using namespace scpar;

FindSuspensionTime::FindSuspensionTime(SplitWaitBlocks::entryFunctionGraphMapType entryFunctionGraphMap, raw_ostream& os) 
: _entryFunctionGraphMap(entryFunctionGraphMap)
, _os(os){
	
}

FindSuspensionTime::~FindSuspensionTime(){

}

bool FindSuspensionTime::isTimedWait(Optional <CFGStmt> waitStmt) {

	Stmt *stmt = const_cast<Stmt*>(waitStmt->getStmt());
	if(CXXMemberCallExpr *ce = dyn_cast<CXXMemberCallExpr>(stmt)) {
		if(ce->getNumArgs() > 1) {
			return true;
		}
	}
	return false;
}


bool FindSuspensionTime::isDeltaWait(Optional <CFGStmt> waitStmt) {

	Stmt *stmt = const_cast<Stmt*>(waitStmt->getStmt());
	if(CXXMemberCallExpr *ce = dyn_cast<CXXMemberCallExpr>(stmt)) {
		if(ce->getNumArgs() == 1) {
			return true;
		}
	}
	
	return false;
}

bool FindSuspensionTime::isEventWait(Optional <CFGStmt> waitStmt) {
		
		Stmt *stmt = const_cast<Stmt*>(waitStmt->getStmt());
		if(CXXMemberCallExpr *ce = dyn_cast<CXXMemberCallExpr>(stmt)) {
			if(ce->getNumArgs() == 1) {
				return true;
			}
		}
		
	return false;
}

unsigned int FindSuspensionTime::getTimeArg(Optional <CFGStmt> waitStmt) {
	
	Stmt *stmt = const_cast<Stmt*>(waitStmt->getStmt());
	if(CXXMemberCallExpr *ce = dyn_cast<CXXMemberCallExpr>(stmt)) {
		FindArgument fa(ce->getArg(0)->IgnoreImpCasts());
		return (atoi(fa.getArgumentName().c_str()));
	}	
	return 0;
}

void FindSuspensionTime::calculateSuspensionTiming(){
	for (SplitWaitBlocks::entryFunctionGraphMapType::iterator it = 
	_entryFunctionGraphMap.begin(), eit = _entryFunctionGraphMap.end();
	it != eit;
	it++) {
		unsigned int localSimTime = 0;
		unsigned int localDeltaTime = 0;
		waitBlockIDSuspensionMapType _waitBlockIDSuspensionMap;

		EntryFunctionContainer *entry = it->first;
		Graph* currentGraph = it->second;
		Graph::adjMapType currentAdjMap = currentGraph->returnAdjList();
		for (Graph::adjMapType::iterator ait = currentAdjMap.begin(), aite = 
		currentAdjMap.end(); ait != aite; ait++) {

			Graph::twoIntPairType nodeIDs = ait->first;
			
			int sourceID = nodeIDs.first;
			int destID = nodeIDs.second;
			
			if(sourceID == 0) {
				vector<timePairType> tmp;
				tmp.push_back(timePairType(0,0));
				_waitBlockIDSuspensionMap.insert(waitBlockIDSuspensionPairType(sourceID, 
				tmp));
			}
			
			Edge *e = ait->second;			
			Optional <CFGStmt> waitStmt = e->getWaitStmt();
			int simTime;
			if(isTimedWait(waitStmt)) {
				timeAdvanceVectorType currTimePair;
				if(_waitBlockIDSuspensionMap.find(sourceID) != _waitBlockIDSuspensionMap.end()) {					
					waitBlockIDSuspensionMapType::iterator waitBlockIDFound = _waitBlockIDSuspensionMap.find(sourceID);
					timeAdvanceVectorType foundTimePair = waitBlockIDFound->second;
					for (int i = 0; i<foundTimePair.size(); i++) {
						simTime = foundTimePair.at(i).first + getTimeArg(waitStmt);
						currTimePair.push_back(timePairType(simTime,0)); 
					}
				}
				_waitBlockIDSuspensionMap.insert(waitBlockIDSuspensionPairType(destID, currTimePair));	
				_edgeSuspensionMap.insert(edgeSuspensionPairType(e, timePairType(simTime, 0)));
			}

			else {
				_os <<"\n Something is terribly wrong ";
			}
		}	
	}
}

FindSuspensionTime::edgeSuspensionMapType FindSuspensionTime::returnEdgeSuspensionMap() {
	return _edgeSuspensionMap;
}

void FindSuspensionTime::dump() {
	for (SplitWaitBlocks::entryFunctionGraphMapType::iterator it = _entryFunctionGraphMap.begin(), eit = _entryFunctionGraphMap.end();
	it != eit;
	it++) {
		_os <<"\n For entry function : " <<it->first->getName();
		
		Graph* g = it->second;
		Graph::adjMapType adjMap = it->second->returnAdjList();

		for (Graph::adjMapType::iterator git = adjMap.begin(), gite= adjMap.end();
		git != gite;
		git++) {
			
			Graph::twoIntPairType nodeConnection = git->first;
			_os <<"\n For connection between " <<nodeConnection.first<<" " <<nodeConnection.second<<"\n";
			_os <<"\n Time advance : \n" ;
			Edge *e = git->second;
			
			if(_edgeSuspensionMap.find(e) != _edgeSuspensionMap.end()) {
				edgeSuspensionMapType::iterator edgeFound = _edgeSuspensionMap.find(e);
				_os <<"      " <<edgeFound->second.first<<" " <<edgeFound->second.second<<"\n";
			}		
		}	
	}		
}
