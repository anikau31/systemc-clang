#include "EntryFunctionContainer.h"
#include "enums.h"

using namespace scpar;

EntryFunctionContainer::~EntryFunctionContainer() {
  // DO NOT free anything.
}

EntryFunctionContainer::EntryFunctionContainer()
: _entryName("NONE")
, _procType(NONE)
, _entryMethodDecl(NULL)
, _constructorStmt(NULL) {

}

EntryFunctionContainer::EntryFunctionContainer( string n, PROCESS_TYPE p, CXXMethodDecl* d, Stmt* s)
: _entryName(n)
, _procType(p)
, _entryMethodDecl(d)
, _constructorStmt(s) {
  
}


EntryFunctionContainer::EntryFunctionContainer( const EntryFunctionContainer& from ) {
  _entryName = from._entryName;
  _procType = from._procType;
  _entryMethodDecl = from._entryMethodDecl;
  _constructorStmt = from._constructorStmt;
	
}

///////////////////////////////////////////////////////////////////////////////////////////////
string EntryFunctionContainer::getName() {
  return _entryName;
}

CXXMethodDecl* EntryFunctionContainer::getEntryMethod() {
  return _entryMethodDecl;
}

Stmt* EntryFunctionContainer::getConstructorStmt() {
  return _constructorStmt;  
}

EntryFunctionContainer::waitContainerListType EntryFunctionContainer::getWaitCalls() {
  return _waitCalls;  
}

PROCESS_TYPE EntryFunctionContainer::getProcessType() {
  return _procType;
}

FindWait::processWaitEventMapType EntryFunctionContainer::getEventWaitMap() {
	return _processWaitEventMap;
}

FindNotify::processNotifyEventMapType EntryFunctionContainer::getEventNotifyMap() {
	return _processNotifyEventMap;
}

FindWait::waitTimesMapType EntryFunctionContainer::getWaitTimesMap() {
	return _waitTimesMap;
}

FindMethodLocalVariables::classMemberMapType EntryFunctionContainer::getMethodLocalVariables(){
	return _inMethodLocalVariables;
}

FindWait::dataTimesMapType EntryFunctionContainer::getDataTimesMap() {
	return _dataTimesMap;
}

FindWait::waitNestLoopMapType EntryFunctionContainer::getWaitNestLoopMap() {
	return _waitNestLoopMap;
}

FindWait::dataNestLoopMapType EntryFunctionContainer::getDataNestLoopMap() {
	return _dataNestLoopMap;
}

FindThreadHierarchy::forStmtThreadHierarchyMapType EntryFunctionContainer::getThreadHierarchyMap() {
	return _forStmtThreads;
}

FindLoopTime::forStmtLoopTimeMapType EntryFunctionContainer::getLoopTimeMap() {
	return _forStmtLoopTimeMap;
}

FindSocketTransportType::socketTransportMapType EntryFunctionContainer::getSocketTransportMap() {
	return _socketTransportMap;
}

FindPayloadCharacteristics::payloadCharacteristicsMapType EntryFunctionContainer::getPayloadCharacteristicsMap() {
	return _payloadCharacteristicsMap;
}

///////////////////////////////////////////////////////////////////////////////////////////////
void EntryFunctionContainer::setConstructorStmt(Stmt* s) {
  _constructorStmt = s;
}

void EntryFunctionContainer::setName(string n) {
  _entryName = n;
}

void EntryFunctionContainer::setProcessType(PROCESS_TYPE p) {
  _procType = p;
}

void EntryFunctionContainer::setEntryMethod(CXXMethodDecl* d) {
  _entryMethodDecl = d;
}

void EntryFunctionContainer::addNotifys(FindNotify& n) {
	_processNotifyEventMap = n.getProcessNotifyEventMap();
}

void EntryFunctionContainer::addPortTrace(FindPortTracing& p) {
	_portTraceMap = p.getPortTraceMap();
}

void EntryFunctionContainer::addLocalVariables(FindMethodLocalVariables& v) {
	_inMethodLocalVariables = v.getInClassMembers();
}

void EntryFunctionContainer::addWaits( FindWait& f) {
  FindWait::waitListType wcalls = f.getWaitCalls();
  for (FindWait::waitListType::iterator it = wcalls.begin(), eit = wcalls.end();
       it != eit;
       it++) {
    // 'it' points to CXXMembercallExpr type.
    WaitContainer* wc = new WaitContainer(f.getEntryMethod(), *it);
    _waitCalls.push_back(wc);
  }
	_processWaitEventMap = f.getProcessWaitEventMap();
	_waitTimesMap = f.getWaitTimesMap();
	_dataTimesMap = f.getDataTimesMap();
	_waitNestLoopMap = f.getWaitNestLoopMap();
	_dataNestLoopMap = f.getDataNestLoopMap();

}

void EntryFunctionContainer::addThreadHierarchy(FindThreadHierarchy& t) {
	_forStmtThreads = t.returnThreadHierarchy();
}

void EntryFunctionContainer::addLoopTime(FindLoopTime &l) {
	_forStmtLoopTimeMap = l.returnLoopTimeMap();
}

void EntryFunctionContainer::addSocketTransportMap(FindSocketTransportType &s) {
	_socketTransportMap = s.returnSocketTransportMap();
}

void 
EntryFunctionContainer::addPayloadCharacteristicsMap(FindPayloadCharacteristics &p) 
{
	_payloadCharacteristicsMap = p.returnPayloadCharacteristicsMap();
}
///////////////////////////////////////////////////////////////////////////////////////////////
void EntryFunctionContainer::dump(raw_ostream& os, int tabn) {

  os <<"\n";
  for (int i = 0; i < tabn; i++) {
    os << " ";
  }
 
  os << "EntryFunctionContainer '" << getName() << "' processType '";
  switch(getProcessType()) {
    case THREAD: os << "SC_THREAD' ";
      break;
    case METHOD: os << "SC_METHOD' ";
      break;
    case CTHREAD: os << "SC_CTHREAD' ";
      break;
    default: os << "NONE' ";
      break;
  }

  os << " CXXMethodDecl '" << getEntryMethod() << "\n";
    int newTabn = ++tabn;
  for (waitContainerListType::iterator it = _waitCalls.begin(), eit = _waitCalls.end();
       it != eit;
        it++) {
  
    (*it)->dump(os, newTabn);
  }

	os <<"\n Local Variables : " ;
	for(FindMethodLocalVariables::classMemberMapType::iterator it = 
	_inMethodLocalVariables.begin(),
	eit = _inMethodLocalVariables.end();
	it != eit;
	it++) {		
		os <<"\n Variable Name : " <<it->first;
		VariableTypeInfo tmp = it->second;
		os<<"\n Variable info : "; 
		tmp.print();
	}

	// The below code causes seg fault. Need to investigate 
	// why?????????????????????????????

/*
	os<<"\n Socket Transport : " ;
	for (FindSocketTransportType::socketTransportMapType::iterator it = 
	_socketTransportMap.begin(),
	eit = _socketTransportMap.end();
	it != eit;
	it++) {
		os <<"\n Socket : " <<it->first<<" Interface details : " ;
		it->second->dump(os);
	}
*/ 
  // TODO
}
