#include "EntryFunctionContainer.h"
#include "enums.h"

using namespace scpar;

EntryFunctionContainer::~EntryFunctionContainer ()
{
	// DO NOT free anything.
}

EntryFunctionContainer::EntryFunctionContainer ():_entryName ("NONE"), _procType (NONE), _entryMethodDecl (NULL)
{

}

EntryFunctionContainer::EntryFunctionContainer (string n, PROCESS_TYPE p, CXXMethodDecl * d, Stmt * s)
:_entryName (n), _procType (p), _entryMethodDecl (d)
{

}


EntryFunctionContainer::EntryFunctionContainer (const EntryFunctionContainer & from)
{
	_entryName = from._entryName;
	_procType = from._procType;
	_entryMethodDecl = from._entryMethodDecl;
}

///////////////////////////////////////////////////////////////////////////////////////////////
string EntryFunctionContainer::getName ()
{
	return _entryName;
}

FindSensitivity::senseMapType EntryFunctionContainer::getSenseMap() {
 return _senseMap;
}

CXXMethodDecl* EntryFunctionContainer::getEntryMethod ()
{
	return _entryMethodDecl;
}

EntryFunctionContainer::waitContainerListType EntryFunctionContainer::getWaitCalls ()
{
	return _waitCalls;
}

EntryFunctionContainer::notifyContainerListType EntryFunctionContainer::getNotifyCalls() {
 return _notifyCalls;
}

PROCESS_TYPE EntryFunctionContainer::getProcessType ()
{
	return _procType;
}

SuspensionAutomata::susCFGVectorType EntryFunctionContainer::getSusCFG() {
  return _susCFG;
	//return _instanceSusCFGMap[numInstance];
}

SuspensionAutomata::transitionVectorType EntryFunctionContainer::getSusAuto() {
 return _susAuto;
 //return _instanceSautoMap[numInstance];
}
///////////////////////////////////////////////////////////////////////////////////////////////
void EntryFunctionContainer::setName (string n)
{
	_entryName = n;
}

void EntryFunctionContainer::setProcessType (PROCESS_TYPE p)
{
	_procType = p;
}

void EntryFunctionContainer::setEntryMethod (CXXMethodDecl * d)
{
	_entryMethodDecl = d;
}

void EntryFunctionContainer::addSensitivityInfo(FindSensitivity &s) {
 _senseMap = s.getSenseMap();
 
}


void EntryFunctionContainer::addWaits (FindWait & f)
{
	FindWait::waitListType wcalls = f.getWaitCalls ();
	for (FindWait::waitListType::iterator it = wcalls.begin (), eit =
			 wcalls.end (); it != eit; it++)
		{
			// 'it' points to CXXMembercallExpr type.
			WaitContainer *
				wc = new WaitContainer (f.getEntryMethod (), *it);

			_waitCalls.push_back (wc);
		}
	//_processWaitEventMap = f.getProcessWaitEventMap();

}

void EntryFunctionContainer::addSusCFGAuto(SuspensionAutomata &s) {
 _susCFG = s.getSusCFG();
 _susAuto = s.getSauto();

 /*
 if (_instanceSusCFGMap.find(numInstance) == _instanceSusCFGMap.end() && _instanceSautoMap.find(numInstance) == _instanceSautoMap.end()) {
 		
 		_instanceSusCFGMap.insert(instanceSusCFGPairType(numInstance, s.getSusCFG()));
		_instanceSautoMap.insert(instanceSautoPairType(numInstance, s.getSauto()));
 }
 */
}

void EntryFunctionContainer::addNotifys( FindNotify& f) {
  FindNotify::NotifyCallListType ncalls = f.getNotifyCallList();
  for (FindNotify::NotifyCallListType::iterator it = ncalls.begin(), eit = ncalls.end();
       it != eit;
       it++) {
    // 'it' points to CXXMembercallExpr type.
    NotifyContainer* nc = new NotifyContainer(f.getEntryMethod(), *it);
    _notifyCalls.push_back(nc);
  }
	//_processWaitEventMap = f.getProcessWaitEventMap();
	
}


///////////////////////////////////////////////////////////////////////////////////////////////
void EntryFunctionContainer::dumpSusCFG(raw_ostream & os)
{

	os << "\n#############################################";    
  SuspensionAutomata::susCFGVectorType susCFGVector = _susCFG;

  for (unsigned int i = 0; i < susCFGVector.size(); i++) {
  	os << "\n Block ID : " << susCFGVector.at(i)->getBlockID();
    os << "\n Is Wait Block : " << susCFGVector.at(i)->isWaitBlock();
    if (susCFGVector.at(i)->getParentBlockID()) {
    	os << "\n Parent ID : " << susCFGVector.at(i)->getParentBlockID();
     	SusCFG *parentBlock = susCFGVector.at(i)->getParentSusCFGBlock();

     	os << "\n Parent Block ID : " << parentBlock->getBlockID();
      os << "\n Size of Children : " << parentBlock->getChildBlockList().size();
    }
    vector < SusCFG * >predBlocks = susCFGVector.at(i)->getPredBlocks();
    vector < SusCFG * >succBlocks = susCFGVector.at(i)->getSuccBlocks();
    os << "\n Predecessor Blocks : ";
    for (unsigned int j = 0; j < predBlocks.size(); j++) {
    	if (predBlocks.at(j)) {
      	os << predBlocks.at(j)->getBlockID() << " ";
      }
    }
    os << "\n Successor Blocks : ";
    for (unsigned int j = 0; j < succBlocks.size(); j++) {
    	if (succBlocks.at(j)) {
      	os << succBlocks.at(j)->getBlockID() << " ";
      }
    }    
  } 	
}

void EntryFunctionContainer::dumpSauto(raw_ostream & os)
{
   vector < Transition * >transitionVector = _susAuto; 
   os << "\n Size of transitionVector : " << transitionVector.size();
   for (unsigned int i = 0; i < transitionVector.size(); i++) {
     Transition *t = transitionVector.at(i);
     t->dump(os);
   }
}     



void EntryFunctionContainer::dump (raw_ostream & os, int tabn)
{
	os << "\n";
	for (int i = 0; i < tabn; i++)
		{
			os << " ";
		}

	os << "EntryFunctionContainer '" << getName () << "' processType '";
	switch (getProcessType ())
		{
		case THREAD:
			os << "SC_THREAD' ";
			break;
		case METHOD:
			os << "SC_METHOD' ";
			break;
		case CTHREAD:
			os << "SC_CTHREAD' ";
			break;
		default:
			os << "NONE' ";
			break;
		}

	os << " CXXMethodDecl '" << getEntryMethod () << "\n";
	int newTabn = ++tabn;
 
 os <<" Wait Calls \n";
	for (waitContainerListType::iterator it = _waitCalls.begin (), eit =
			 _waitCalls.end (); it != eit; it++)
		{

			(*it)->dump (os, newTabn);
		}
  os <<" Notify Calls \n";
 for (notifyContainerListType::iterator it = _notifyCalls.begin(), eit = _notifyCalls.end();
     it != eit;
     it++ ) {
  (*it)->dump(os, newTabn);
 }
  
 os <<"\n Suspension CFG";
 os <<"\n ###############";
 dumpSusCFG(os);
 os <<"\n ###############";
 os <<"\n Suspension Automaton";
 os <<"\n @@@@@@@@@@@@@@@@";
 dumpSauto(os); 
 os <<"\n @@@@@@@@@@@@@@@@";
}
