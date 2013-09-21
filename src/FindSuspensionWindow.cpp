#include "FindSuspensionWindow.h"
#include "FindVariable.h"
#include "FindArgument.h"
using namespace scpar;

//////////////////////////////////////////////////////////////////////////////////

VariableAccess::VariableAccess(const string& variableName, bool writeStatus, bool 
readStatus) 
:	_variableName(variableName)
, _writeStatus(writeStatus)
, _readStatus(readStatus) {

}
VariableAccess::~VariableAccess() {

}

bool VariableAccess::returnWriteStatus() {
	return _writeStatus;
}

bool VariableAccess::returnReadStatus() {
	return _readStatus;
}

string VariableAccess::returnVariableName() {
	return _variableName;
}

//////////////////////////////////////////////////////////////////////////////////
SuspensionWindow::SuspensionWindow(CXXMemberCallExpr* memberExpr, unsigned int simulationTime, unsigned int deltaTime, bool complete, bool eventWait)
	: _memberExpr(memberExpr)
	, _simulationTime(simulationTime) 
	, _deltaTime(deltaTime) 
	, _complete(complete)
	, _eventWait(eventWait)
	{
	
}

SuspensionWindow::SuspensionWindow(CXXMemberCallExpr* memberExpr, unsigned int simulationTime, unsigned int deltaTime, bool complete)
	: _memberExpr(memberExpr)
	, _simulationTime(simulationTime) 
	, _deltaTime(deltaTime) 
	, _complete(complete)
	{
	
}

SuspensionWindow::~SuspensionWindow() {

}

CXXMemberCallExpr* SuspensionWindow::returnMemberExpr(){
	return _memberExpr;
}
unsigned int SuspensionWindow::returnSimTime() {
	return _simulationTime;
}
unsigned int SuspensionWindow::returnDeltaTime(){
	return _deltaTime;
}
bool SuspensionWindow::returnStatus() {
	return _complete;
}
bool SuspensionWindow::isWaitEvent() {
	return _eventWait;
}

//////////////////////////////////////////////////////////////////////////////
FindSuspensionWindow::FindSuspensionWindow(vector<EntryFunctionContainer*> ve, ASTContext* a, raw_ostream& os)
  : _vef(ve)
  , _cfg(NULL)
  , _os(os)
	, _a(a){

}

FindSuspensionWindow::~FindSuspensionWindow() {
  
	delete _cfg;
	for(waitNotifyTimeWindowMapType::iterator it = _waitTimeWindowMap.begin(), eit = _waitTimeWindowMap.end();
	it != eit;
	it++) {
		delete it->second;
	}
	_waitTimeWindowMap.clear();

	for(waitNotifyTimeWindowMapType::iterator it = _notifyTimeWindowMap.begin(), eit = _notifyTimeWindowMap.end();
	it != eit;
	it++) {
		delete it->second;
	}
	_notifyTimeWindowMap.clear();

}

CXXMemberCallExpr* FindSuspensionWindow::isWaitCall(const Stmt *cs) {

  Stmt* s = const_cast<Stmt*>(cs);
  CXXMemberCallExpr* m = dyn_cast<CXXMemberCallExpr>(s);
  if (!m) {
    return NULL;
  }
  
  // _os << "**************** CHECK FOR WAIT ************ : " << waitCalls.size() << "\n";
	for(int i = 0 ; i<_vef.size(); i++) {
		EntryFunctionContainer *_ef = _vef.at(i);
  	EntryFunctionContainer::waitContainerListType waitCalls = _ef->getWaitCalls();
  	for (EntryFunctionContainer::waitContainerListType::iterator it = waitCalls.begin(), eit = waitCalls.end();
     	  it != eit;
     	  ++it ) {
    	WaitContainer* w = (*it);
    	//    _os << "m: " << m << ", w: " << w->getASTNode() << "\n";
    	if (m == w->getASTNode()) {
     	 //    _os << "FOUND THE WAIT\n";
     	 //  m->dump();
      	return m;
    	}	 
  	}
	}
}

CXXMemberCallExpr* FindSuspensionWindow::isNotifyCall(const Stmt *cs) {

  Stmt* s = const_cast<Stmt*>(cs);
	 if (!s) {
    //    s->dump(); 
  }
  CXXMemberCallExpr* m = dyn_cast<CXXMemberCallExpr>(s);
  if (!m) {
    return NULL;
  }
	if(m->getMethodDecl()->getNameAsString() == string("notify")) {
		return m;
	}
	else {
		return NULL;
	}
}

void FindSuspensionWindow::addLeftVariable(string variableName) {
	
	VariableAccess *lhs;	
	if(_variableAccessMap.find(variableName) == _variableAccessMap.end()) {
		lhs = new VariableAccess(variableName, true, false);			
	}
	else {
		variableAccessMapType::iterator foundVariable = 
		_variableAccessMap.find(variableName);
		lhs = new VariableAccess(variableName, true, 
		foundVariable->second->returnReadStatus());
		_variableAccessMap.erase(variableName);		
	}
	_variableAccessMap.insert(variableAccessPairType(variableName, lhs));
	_waitVariables.push_back(lhs);
}

void FindSuspensionWindow::addRightVariable(string variableName) {

	VariableAccess *rhs;
	if(_variableAccessMap.find(variableName) == _variableAccessMap.end()) {
		rhs = new VariableAccess(variableName, false, true);		
	}
	else {
		variableAccessMapType::iterator foundVariable = 
		_variableAccessMap.find(variableName);
		rhs = new VariableAccess(variableName, foundVariable->second->returnWriteStatus(), true);		
		_variableAccessMap.erase(variableName);		
	}
	_variableAccessMap.insert(variableAccessPairType(variableName, rhs));
	_waitVariables.push_back(rhs);
}

void FindSuspensionWindow::addVariable(string variableName) {
	
	VariableAccess *rhs;
	if(_variableAccessMap.find(variableName) == _variableAccessMap.end()) {
		rhs = new VariableAccess(variableName, true, true);			
		_variableAccessMap.insert(variableAccessPairType(variableName, rhs));
	}
	else {
		variableAccessMapType::iterator foundVariable = 
		_variableAccessMap.find(variableName);
		rhs = new VariableAccess(variableName, true, true);		
		_variableAccessMap.erase(variableName);
		_variableAccessMap.insert(variableAccessPairType(variableName, rhs));
	}
	_waitVariables.push_back(rhs);
}

void FindSuspensionWindow::addVariableAccessType(Stmt *stmt) {

	if(BinaryOperator *bo = dyn_cast<BinaryOperator>(stmt)) {				
		FindVariable lv(bo->getLHS()->IgnoreImpCasts());
		if(lv.getArgumentName() != "NONE") {
			addLeftVariable(lv.getArgumentName());
		}	
		FindVariable rv(bo->getRHS()->IgnoreImpCasts());
		if(rv.getArgumentName()!= "NONE") {
			addRightVariable(rv.getArgumentName());
		}
	}
	else if(UnaryOperator *uo = dyn_cast<UnaryOperator>(stmt)) {					
		FindVariable v(uo->getSubExpr()->IgnoreImpCasts());
		if(v.getArgumentName()!= "NONE") {
			addVariable(v.getArgumentName());
		}
	}
}

void FindSuspensionWindow::updateWaitTime(CXXMemberCallExpr *waitExpr, unsigned int *simTime, 
																					unsigned int *deltaTime, bool *waitEventFound) {
	
	SuspensionWindow *s;
	if(waitExpr->getNumArgs() > 1) { // timed wait
		FindArgument fa(waitExpr->getArg(0)->IgnoreImpCasts());							
		*simTime = (*simTime) + atoi(fa.getArgumentName().c_str());
		if(*waitEventFound == true) {
			_waitsAfterEventVector.push_back(waitExpr);													
			s = new SuspensionWindow(waitExpr, (*simTime), 0, false, false);
		}
		else {
			s = new SuspensionWindow(waitExpr, (*simTime), 0, true, false);	
		}
		_waitTimeWindowMap.insert(waitNotifyTimeWindowPairType(waitExpr, s));
	}
	else { // delta or event wait
		FindArgument fa(waitExpr->getArg(0)->IgnoreImpCasts());
		*deltaTime = (*deltaTime) + 1;
		if(fa.getArgumentName() == "SC_ZERO_TIME") {			
				if(*waitEventFound == true) {
					_waitsAfterEventVector.push_back(waitExpr);
					s = new SuspensionWindow(waitExpr, (*simTime), (*deltaTime), false, false);
				}
				else {
					s = new SuspensionWindow (waitExpr, (*simTime), (*deltaTime), true, false);
				}
				_waitTimeWindowMap.insert(waitNotifyTimeWindowPairType(waitExpr, s));
		}
		else {
			if(*waitEventFound == false) {
				*waitEventFound = true;
				currExpr = waitExpr;				
			}
			else {
			FindArgument fv(currExpr->getArg(0)->IgnoreImpCasts());
			_waitsAfterEventMap.insert(waitsAfterEventPairType(fv.getArgumentName(), _waitsAfterEventVector));	
			_waitsAfterEventVector.clear();
			currExpr = waitExpr;
			}
		}
	}
}

void FindSuspensionWindow::updateNotifyTime(CXXMemberCallExpr *notifyExpr, unsigned int *simTime, 																						unsigned int *deltaTime, bool *waitEventFound) {
	
	SuspensionWindow *s;
	if(*waitEventFound == true) {
		_waitsAfterEventVector.push_back(notifyExpr);
		s = new SuspensionWindow(notifyExpr, (*simTime), (*deltaTime), false);
	}
	else {
		s = new SuspensionWindow(notifyExpr, (*simTime), (*deltaTime), true);
		_cleanNotify.push_back(s);
	}
	_notifyTimeWindowMap.insert(waitNotifyTimeWindowPairType(notifyExpr, s));
}

void FindSuspensionWindow::traverseCFG() {

  LangOptions LO;
  LO.CPlusPlus = true;
  PrintingPolicy Policy(LO);

	const CFG::BuildOptions& b= CFG::BuildOptions();

	for (int i = 0; i<_vef.size(); i++) {

		EntryFunctionContainer *currentFunction = _vef.at(i);
		CXXMethodDecl *d = currentFunction->getEntryMethod();
		_cfg = CFG::buildCFG(cast<Decl>(d), d->getBody(), _a, b);
		FindWait::waitTimesMapType waitTimesMap = currentFunction->getWaitTimesMap();	
		
		bool waitEventFound = false;
		CXXMemberCallExpr *eventWait = NULL;
		unsigned int simTime = 0;
		unsigned int deltaTime = 0;
			
		for(CFG::iterator it = _cfg->end()-1, eit = _cfg->begin(); //had to do this 
				it != eit;
				--it) {

			const CFGBlock *block = *it;			
			for(CFGBlock::const_iterator bit = block->begin(), bite = block->end(); 
					bit != bite;
					bit++) {
				
				if(Optional<CFGStmt> cs = bit->getAs<CFGStmt>()) {
					const Stmt *s = cs->getStmt();
					Stmt *stmt = const_cast<Stmt*>(s);
					
					addVariableAccessType(stmt);
					
					if(isWaitCall(s) != NULL) { // wait found
						CXXMemberCallExpr *waitExpr = isWaitCall(s);						
						updateWaitTime(waitExpr, &simTime, &deltaTime, &waitEventFound);
						_waitVariablesMap.insert(waitNotifyVariablesPairType(waitExpr, _waitVariables));
						_waitVariables.clear();
					}
					if(isNotifyCall(s) != NULL) {
						CXXMemberCallExpr* notifyExpr = isNotifyCall(s);
						updateNotifyTime(notifyExpr, &simTime, &deltaTime, &waitEventFound);
					}
				}
			}
		}	
		FindArgument fv(currExpr->getArg(0)->IgnoreImpCasts());
		_waitsAfterEventMap.insert(waitsAfterEventPairType(fv.getArgumentName(), _waitsAfterEventVector));	
	}

	// Update timing 
	while(_cleanNotify.size() != 0) {
		SuspensionWindow *tmp = _cleanNotify.back();
		_cleanNotify.pop_back();
		FindArgument fa(tmp->returnMemberExpr()->getCallee()->IgnoreImpCasts());
		if(_waitsAfterEventMap.find(fa.getArgumentName()) != 
		_waitsAfterEventMap.end()) {
			waitsAfterEventMapType::iterator waitsFound = _waitsAfterEventMap.find(fa.getArgumentName());				
			vector<CXXMemberCallExpr*> tmpVector = waitsFound->second;			
			for (int i = 0; i<tmpVector.size(); i++) {				
				if(_waitTimeWindowMap.find(tmpVector.at(i)) != _waitTimeWindowMap.end()) {
					waitNotifyTimeWindowMapType::iterator waitFound = 
					_waitTimeWindowMap.find(tmpVector.at(i));
					SuspensionWindow *tmpWait = waitFound->second;
					unsigned int tmpSimTime = tmpWait->returnSimTime();
					unsigned int tmpDeltaTime = tmpWait->returnDeltaTime();
					tmpSimTime += tmp->returnSimTime();
					tmpDeltaTime += tmp->returnDeltaTime();
					SuspensionWindow *snew = new SuspensionWindow(tmpVector.at(i), 
					tmpSimTime, tmpDeltaTime, true, true);
					_waitTimeWindowMap.erase(tmpVector.at(i));
					_waitTimeWindowMap.insert(waitNotifyTimeWindowPairType(tmpVector.at(i), 
					snew));
				}	
				else if (_notifyTimeWindowMap.find(tmpVector.at(i)) != 
				_notifyTimeWindowMap.end()) {
					waitNotifyTimeWindowMapType::iterator notifyFound = 
					_notifyTimeWindowMap.find(tmpVector.at(i));
					SuspensionWindow *tmpNotify = notifyFound->second;
					unsigned int tmpSimTime = tmpNotify->returnSimTime();
					unsigned int tmpDeltaTime = tmpNotify->returnDeltaTime();
					tmpSimTime += tmp->returnSimTime();
					tmpDeltaTime += tmp->returnDeltaTime();
					SuspensionWindow *snew = new SuspensionWindow(tmpVector.at(i), 
					tmpSimTime, tmpDeltaTime, true, true);
					_notifyTimeWindowMap.erase(tmpVector.at(i));
					_notifyTimeWindowMap.insert(waitNotifyTimeWindowPairType(tmpVector.at(i), 
					snew));				
					_cleanNotify.push_back(snew);
				}
			}
		}
	}
}

FindSuspensionWindow::waitNotifyVariablesMapType FindSuspensionWindow::getWaitVariableMap() {
	return _waitVariablesMap;
}


void FindSuspensionWindow::dump() {
	
	for(waitNotifyTimeWindowMapType::iterator it = _waitTimeWindowMap.begin(), eit = 
	_waitTimeWindowMap.end();
	it != eit;
	it++) {
		FindArgument fa(it->first->getArg(0)->IgnoreImpCasts());
		_os <<"\n For wait call : " <<it->first<<" with first argument : " 
		<<fa.getArgumentName();
		_os <<" Sim Time : " <<it->second->returnSimTime()<<" Delta time : " 
		<<it->second->returnDeltaTime();
	}
////////////////////////////////////////////////////////////////////////////////////////////	
	for (waitNotifyVariablesMapType::iterator it = _waitVariablesMap.begin(), eit = _waitVariablesMap.end();
	it != eit;
	it++) {
		_os <<"\n For WaitExpr : " <<it->first<<" Variables and access types : " ;
		vector<VariableAccess*> tmp = it->second;
		for (unsigned int i = 0 ; i< tmp.size(); i++) {
			_os <<"\n   Variable Name : " <<tmp.at(i)->returnVariableName()<<" Write : " <<tmp.at(i)->returnWriteStatus()<<" Read : " <<tmp.at(i)->returnReadStatus();
		}
	}
}
