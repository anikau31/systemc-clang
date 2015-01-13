#include "GlobalSuspensionAutomata.h"
using namespace scpar;

GlobalSuspensionAutomata::GlobalSuspensionAutomata(Model * systemCmodel,
                                                   raw_ostream & os,
                                                   ASTContext * a)
:_systemcModel(systemCmodel), _os(os), _a(a)
{
}

GlobalSuspensionAutomata::~GlobalSuspensionAutomata()
{
}


void GlobalSuspensionAutomata::initializeGpuMap() {
 // GPU map algorithm. 
 // Input is the global sauto
 // First identify transitions with the same time-stamp
 // Need data structures of the form <time-stamp, vector of dp segments>
 // Using the above data structure, for each time-stamp set, we identify which ones will go on the gpu.
 //_os <<"\n Initialize gpu map\n";

 for (int i = 0; i<_globalSauto.size(); i++) {
  Transition *aTransition = _globalSauto.at(i); 
  for (int j = 0; j<_globalSauto.size(); j++) {
   Transition *bTransition = _globalSauto.at(j);
   if (aTransition != bTransition) {
	   transitionTimeMapType::iterator aTransitionFound = _transitionTimeMap.find(aTransition);
	   transitionTimeMapType::iterator bTransitionFound = _transitionTimeMap.find(bTransition);
	   timePairType aTimePair = aTransitionFound->second;
	   timePairType bTimePair = bTransitionFound->second;

	   if (aTimePair.first == bTimePair.first && aTimePair.second == bTimePair.second) {
	    // Transitions with same time-stamp
	    vector<SusCFG*> aCodeBlocks = aTransition->returnCodeBlocks();
	    vector<SusCFG*> bCodeBlocks = bTransition->returnCodeBlocks();
	    vector<SusCFG*> susCFGBlockList;
	    // iterate through each code block set and accumulate DP blocks.    
	    // debug susCFGBlockGPUMacroMap
	    

	     timePairType timePair = make_pair(aTimePair.first, aTimePair.second);
	     if (_commonTimeDPMap.find(timePair) == _commonTimeDPMap.end()) {
					for (int k = 0; k<aCodeBlocks.size(); k++) { 
	     			if (_susCFGBlockGPUMacroMap.find(aCodeBlocks.at(k)) != _susCFGBlockGPUMacroMap.end()) {
	       		// DP segment found										
							//if (notInVector(susCFGBlockList, aCodeBlocks.at(k))){
							susCFGBlockList.push_back(aCodeBlocks.at(k));
							//}
	     			}    
	    		}
	    		for (int k = 0; k<bCodeBlocks.size(); k++) {
	     			if (_susCFGBlockGPUMacroMap.find(bCodeBlocks.at(k)) != _susCFGBlockGPUMacroMap.end()) {
	       		// DP segment found										
	       		//if(notInVector(susCFGBlockList, bCodeBlocks.at(k))) {
							susCFGBlockList.push_back(bCodeBlocks.at(k));
						//}
	     		}
	    	}
	      _commonTimeDPMap.insert(commonTimeDPPairType(timePair, susCFGBlockList));
	     }
	     else {
					commonTimeDPMapType::iterator commonTimeFound = _commonTimeDPMap.find(timePair);
					vector<SusCFG*> tmpVec = commonTimeFound->second;
		    	for (int k = 0; k<bCodeBlocks.size(); k++) {
	     			if (_susCFGBlockGPUMacroMap.find(bCodeBlocks.at(k)) != _susCFGBlockGPUMacroMap.end()) {
	       		// DP segment found										
	       			if(!isElementPresent(tmpVec, bCodeBlocks.at(k))) {
								susCFGBlockList.push_back(bCodeBlocks.at(k));
							}
	     			}
	    		}														
					tmpVec.insert(tmpVec.end(), susCFGBlockList.begin(), susCFGBlockList.end());
					_commonTimeDPMap.erase(timePair);					
					_commonTimeDPMap.insert(commonTimeDPPairType(timePair, tmpVec));
	     }	   
    }
   }
  }
 }

// Actual gpu map algo starts here....
// use _commonTimeDPMap and for each timePair with more than one DP segment, do pseudo-knapsacking algo
/*
	for (commonTimeDPMapType::iterator cit = _commonTimeDPMap.begin(), cite = _commonTimeDPMap.end(); 
								cit != cite;
								cit++) {
	_os <<"\nTime : " <<cit->first.first<<" " <<cit->first.second<<"\n";
	for (int i = 0; i<cit->second.size(); i++) {
		_os <<"\n Block ID: "<<cit->second.at(i)<<" " <<cit->second.at(i)->getBlockID();
	}
}
*/
 for (commonTimeDPMapType::iterator cit = _commonTimeDPMap.begin(), cite = _commonTimeDPMap.end(); 
		 cit != cite;
		 cit++) {
   timePairType timePair = cit->first;
   //_os <<"\n Looking at time-pair : " <<timePair.first<<" " <<timePair.second;
	 vector<SusCFG*> susCFGBlockList = cit->second;
   // l and u are lower and upper limits of the makespan. 
   // c_ideal and c_actual are ideal and actual makespan values.
   float l = 0.0;
   float u = 0.0;
   float c_ideal = u;
   float c_actual = u;
  	 
   for (int i = 0; i<susCFGBlockList.size(); i++) {
   	if (_susCFGBlockGPUMacroMap.find(susCFGBlockList.at(i)) != _susCFGBlockGPUMacroMap.end()) {
    	// Found a DP segment
			susCFGBlockGPUMacroMapType::iterator gpuMacroFound = _susCFGBlockGPUMacroMap.find(susCFGBlockList.at(i));
			
			GPUMacro *gpuMacro = gpuMacroFound->second;
			u = u + max(gpuMacro->getGPUTime(), gpuMacro->getCPUTime());	
			//_os <<"\n Value of u : " <<u;
		}
   } 
	 c_actual = u;
	 while(u != l) {
	   c_ideal = (u+l)/2;
		 //_os <<"\n Value of c_ideal : " <<c_ideal;		 
		 if (GPUMap(c_ideal, susCFGBlockList, c_actual)) {
			//_os <<"\n Changing value of u to : "<<c_actual;
		 	u = c_actual;
		 }
		 else {
			//_os <<"\n Changing value of l to : "<<c_actual;
		 	l = c_actual;
		 }
	 }
 } 
}

bool GlobalSuspensionAutomata::GPUMap(float c_ideal, vector<SusCFG*> susCFGBlockList, float& c_actual) {
	vector<SusCFG*> gpuFitSusCFGVector;
	vector<SusCFG*> gpuUnfitSusCFGVector;
	for (int i = 0; i<susCFGBlockList.size(); i++) {
		if (_susCFGBlockGPUMacroMap.find(susCFGBlockList.at(i)) != _susCFGBlockGPUMacroMap.end()) {
			susCFGBlockGPUMacroMapType::iterator gpuMacroFound = _susCFGBlockGPUMacroMap.find(susCFGBlockList.at(i));
			GPUMacro *gpuMacro = gpuMacroFound->second;
			//_os <<"\n GPU macro gpu time : " <<gpuMacro->getGPUTime()<<" CPU time : " <<gpuMacro->getCPUTime();
		 	 

			if (gpuMacro->getGPUTime() <= c_ideal) {
			  //susCFGBlockList.at(i)->addGPUFit();
				//gpuMacro->addGPUFit();
				//_os <<"\n Pushing block : " <<susCFGBlockList.at(i)<<" into gpuFitSusCFGVector";
				gpuFitSusCFGVector.push_back(susCFGBlockList.at(i));
			}			
			else {
				//susCFGBlockList.at(i)->denyGPUFit();
				//gpuMacro->denyGPUFit();
				gpuUnfitSusCFGVector.push_back(susCFGBlockList.at(i));
			}
		}
	}
	//_os <<"\n arranging susCFGblocks";
  vector<SusCFG*> orderedSusCFGList = arrangeGPUSusCFGBlocks(gpuFitSusCFGVector);
	///_os <<"\n done arranging block list ==>";
	/*
	for (int i = 0; i<orderedSusCFGList.size(); i++) {
		_os <<" "<<orderedSusCFGList.at(i)->getBlockID()<<" "<<orderedSusCFGList.at(i);
	}
	
	_os <<"\n";
	*/
	for (int i = 0; i<orderedSusCFGList.size(); i++) {
		GPUMacro *gpuMacro = _susCFGBlockGPUMacroMap[orderedSusCFGList.at(i)];
		float gpuTime = gpuMacro->getGPUTime();
		//_os <<"\n gpuTime : " <<gpuTime;
		//_os <<"\n c_ideal : " <<c_ideal;
		if (gpuTime <= c_ideal){
			c_ideal = c_ideal - gpuTime;
		}
		else {
			//orderedSusCFGList.at(i)->denyGPUFit();
			gpuUnfitSusCFGVector.push_back(gpuFitSusCFGVector.at(i));
			gpuFitSusCFGVector.erase(gpuFitSusCFGVector.begin()+i-1);
		}
	}
	float newGPUTime = 0;
	float newCPUTime = 0;
	/*
	for (int i = 0; i<susCFGBlockList.size(); i++) {
		if (susCFGBlockList.at(i)->isGPUFit()) {
			// On GPU
			GPUMacro *gpuMacro = _susCFGBlockGPUMacroMap[susCFGBlockList.at(i)];
			newGPUTime += gpuMacro->getGPUTime();
		}
		else {
			// On CPU
			GPUMacro *gpuMacro = _susCFGBlockGPUMacroMap[susCFGBlockList.at(i)];
			if (newCPUTime < gpuMacro->getCPUTime()){
				newCPUTime = gpuMacro->getCPUTime();
			}
		}
	}
	*/
	for (int i = 0; i<gpuFitSusCFGVector.size(); i++) {
		GPUMacro *gpuMacro = _susCFGBlockGPUMacroMap[gpuFitSusCFGVector.at(i)];
		newGPUTime += gpuMacro->getGPUTime();
	}
	for (int i = 0; i<gpuUnfitSusCFGVector.size(); i++) {
		GPUMacro *gpuMacro = _susCFGBlockGPUMacroMap[gpuUnfitSusCFGVector.at(i)];
		if (newCPUTime < gpuMacro->getCPUTime()) {
			newCPUTime = gpuMacro->getCPUTime();
		}
	}

	//_os <<"\n new GPUTime : " <<newGPUTime<<" newCPUTime : " <<newCPUTime;
	if (c_actual > max(newGPUTime, newCPUTime)) {
		c_actual = max(newGPUTime, newCPUTime);
		for (int i = 0; i<gpuFitSusCFGVector.size(); i++) {
			gpuFitSusCFGVector.at(i)->addGPUFit();
			_susCFGBlockGPUMacroMap[gpuFitSusCFGVector.at(i)]->addGPUFit();
		}
		for (int i = 0; i<gpuUnfitSusCFGVector.size(); i++) {
			gpuUnfitSusCFGVector.at(i)->denyGPUFit();
			_susCFGBlockGPUMacroMap[gpuUnfitSusCFGVector.at(i)]->denyGPUFit();
		}
		return true;
	}
	return false;
}

vector<SusCFG*> GlobalSuspensionAutomata::arrangeGPUSusCFGBlocks(vector<SusCFG*> gpuFitSusCFGVector) {
	vector<SusCFG*> leftList;
	vector<SusCFG*> rightList;
	int middle;
	if (gpuFitSusCFGVector.size() <= 1) {
		return gpuFitSusCFGVector;
	}
	else {
	  middle = gpuFitSusCFGVector.size() /2;
		for (int i = 0; i<middle; i++) {
			leftList.push_back(gpuFitSusCFGVector.at(i));
		}
		for (int i= middle; i<gpuFitSusCFGVector.size(); i++) {
			rightList.push_back(gpuFitSusCFGVector.at(i));
		}

		leftList = arrangeGPUSusCFGBlocks(leftList);
		rightList = arrangeGPUSusCFGBlocks(rightList);
		return(merge_sort(leftList, rightList));		
	}
}

// This needs to be a utility function
vector<SusCFG*> GlobalSuspensionAutomata::merge_sort(vector<SusCFG*> leftList, vector<SusCFG*> rightList){
	vector<SusCFG*> orderedList;
	while(leftList.size() > 0 || rightList.size() > 0) {
		if (leftList.size() > 0 && rightList.size() > 0) {
			GPUMacro *leftGPUMacro = _susCFGBlockGPUMacroMap[leftList.back()];
			GPUMacro *rightGPUMacro = _susCFGBlockGPUMacroMap[rightList.back()];
			int leftGPUTime = leftGPUMacro->getGPUTime();
			int leftCPUTime = leftGPUMacro->getCPUTime();
			int rightGPUTime = rightGPUMacro->getCPUTime();
			int rightCPUTime = rightGPUMacro->getCPUTime();
			int leftAccFactor = leftCPUTime/leftGPUTime;
			int rightAccFactor = rightCPUTime/rightGPUTime;
			if (leftAccFactor >= rightAccFactor) {
				orderedList.push_back(leftList.back());
				leftList.pop_back();
			}
			else {
				orderedList.push_back(rightList.back());
				rightList.pop_back();
			}
		}
		else if (leftList.size() > 0) {
			orderedList.push_back(leftList.back());
			leftList.pop_back();
		}
		else if (rightList.size() > 0) {
			orderedList.push_back(rightList.back());
			rightList.pop_back();
		}
	}
	return orderedList;
}


// This needs to be a utility function
float GlobalSuspensionAutomata::max(float a, float b) {
  if (a > b) {
		return a;
	}
	else if (a < b) {
		return b;
	}
	else 
		return a;
}

void GlobalSuspensionAutomata::annotateTransitionsDPSeg(Transition * t, int instanceId)
{

  vector < SusCFG * >codeBlockVector = t->returnCodeBlocks();
  for (int i = 0; i < codeBlockVector.size(); i++) {

    SusCFG *susCFGBlock = codeBlockVector.at(i);
    CFGBlock *cfgBlock = susCFGBlock->getBlock();
    if (cfgBlock->getLoopTarget()) {
      if (const ForStmt * cfs =
          dyn_cast < ForStmt > (cfgBlock->getLoopTarget())) {
        ForStmt *fs = const_cast < ForStmt * >(cfs);
        for (entryFunctionMacroMapType::iterator it =
             _entryFunctionGPUMacroMap.begin(), eit =
             _entryFunctionGPUMacroMap.end(); it != eit; it++) {
          FindGPUMacro::forStmtGPUMacroMapType gpuMacroMap = it->second;

					for (FindGPUMacro::forStmtGPUMacroMapType::iterator fit = gpuMacroMap.begin(), 
													fite = gpuMacroMap.end();
													fit != fite;
													fit++) {
						FindGPUMacro::forStmtInstanceIdPairType forStmtInstanceIdPair = fit->first;
						
						if (forStmtInstanceIdPair.first == instanceId && forStmtInstanceIdPair.second == fs) {
							_susCFGBlockGPUMacroMap.insert(susCFGBlockGPUMacroPairType(susCFGBlock, fit->second));

							break;
						}
					}
        }
      }
    }
  }

}

void GlobalSuspensionAutomata::initialise()
{

  _moduleInstanceMap = _systemcModel->getModuleInstanceMap();
 
	for (Model::moduleInstanceMapType::iterator it = _moduleInstanceMap.begin(), eit =
       _moduleInstanceMap.end(); it != eit; it++) {
    vector<ModuleDecl*> mdVec = it->second;
		
		for (int i = 0; i<mdVec.size(); i++) {
			ModuleDecl *md = mdVec.at(i);			
    	vector < EntryFunctionContainer * >vef = md->getEntryFunctionContainer();

			for (int j = 0; j < vef.size(); j++) {
      	SuspensionAutomata::transitionVectorType sauto =
        	vef.at(j)->getSusAuto();
				
				entryFunctionInstanceIdPairType entryFunctionInstanceIdPair = make_pair(vef.at(j), i);
				_instanceFunctionSautoMap.
          insert(instanceFunctionSautoPairType
                 (entryFunctionInstanceIdPair, sauto));
      }	
			
		  for (int j = 0; j < vef.size(); j++) {
    		CXXMethodDecl *entryFunctionDecl = vef.at(j)->getEntryMethod();

				FindGPUMacro findGPUMacro(entryFunctionDecl, i, _os);

				_entryFunctionGPUMacroMap.insert(entryFunctionMacroPairType(vef.at(j), findGPUMacro.getForStmtGPUMacroMap()));   
    	}			
		}
  }
	
  instanceFunctionSautoMapType::iterator it = _instanceFunctionSautoMap.begin();
  vector < Transition * >transitionVec = it->second;
  State *initialState;
  for (int i = 0; i < transitionVec.size(); i++) {
    Transition *t = transitionVec.at(i);
    State *state = t->returnInitialState();
    if (state->isInitial()) {
      initialState = state;
      break;
    }
  }

  for (instanceFunctionSautoMapType::iterator it =
       _instanceFunctionSautoMap.begin(), eit = _instanceFunctionSautoMap.end();
       it != eit; it++) {

    vector < Transition * >sauto = it->second;
		entryFunctionInstanceIdPairType entryFunctionIdPair = it->first;
    for (int i = 0; i < sauto.size(); i++) {
      Transition *t = sauto.at(i);
			t->addInstanceId(entryFunctionIdPair.second);
      if (t->returnInitialState()->isInitial()) {
        t->addInitialState(initialState);
      }
      _visitTransitionMap.insert(visitTransitionPairType(t, false));
      _globalSauto.push_back(t);
    }
  }

  for (int i = 0; i < _globalSauto.size(); i++) {
    Transition *t = _globalSauto.at(i);
				
    annotateTransitionsDPSeg(t, t->returnInstanceId());
    State *initState = t->returnInitialState();

    vector < Transition * >incomingTransitions;
    vector < Transition * >outgoingTransitions;
    for (int j = 0; j < _globalSauto.size(); j++) {
      Transition *t = _globalSauto.at(j);
      if (t->returnInitialState() == initState) {
        outgoingTransitions.push_back(t);
      }
      if (t->returnFinalState() == initState) {
        incomingTransitions.push_back(t);
      }
    }
    _incomingTransitionsMap.
      insert(stateTransitionsPairType(initState, incomingTransitions));
    _outgoingTransitionsMap.
      insert(stateTransitionsPairType(initState, outgoingTransitions));
  }
}

vector <
  Transition * >GlobalSuspensionAutomata::getOutgoingTransitions(State * state)
{
  if (_outgoingTransitionsMap.find(state) != _outgoingTransitionsMap.end()) {
    stateTransitionsMapType::iterator stateFound =
      _outgoingTransitionsMap.find(state);
    return stateFound->second;
  } else {
    llvm::errs() << "\n ERROR : Could not find state in map";
    exit(0);
  }
}

GlobalSuspensionAutomata::susCFGBlockGPUMacroMapType GlobalSuspensionAutomata::
getSusCFGBlockGPUMacroMap()
{
  return _susCFGBlockGPUMacroMap;
}


vector<Transition*>GlobalSuspensionAutomata::getGlobalSauto() {
	return _globalSauto;
}

GlobalSuspensionAutomata::entryFunctionMacroMapType GlobalSuspensionAutomata::getEntryFunctionMacroMap() {
	return _entryFunctionGPUMacroMap;
}


vector<Transition*> GlobalSuspensionAutomata::getTransitionsAtTime(timePairType tp) {
	vector<Transition*> transitionVec;
	for (transitionTimeMapType::iterator it = _transitionTimeMap.begin(), eit = _transitionTimeMap.end();
									it != eit;
									it++) {
			timePairType timePair = it->second;
			if (timePair.first == tp.first && timePair.second == tp.second) {
				transitionVec.push_back(it->first);
			}
	}
	return transitionVec;
}

GlobalSuspensionAutomata::transitionTimeMapType GlobalSuspensionAutomata::getTransitionTimeMap() {
	return _transitionTimeMap;
}

GlobalSuspensionAutomata::timePairType GlobalSuspensionAutomata::getTimeForTransition(Transition *t) {
	if (_transitionTimeMap.find(t) != _transitionTimeMap.end()) {
		return _transitionTimeMap[t];
	}
	else {
		_os <<"\n Cannot find time of transition : " <<t;
		return make_pair(0, 0);
	}
}



vector <
  Transition * >GlobalSuspensionAutomata::

getIncomingTransitions(State * state)
{
  if (_incomingTransitionsMap.find(state) != _incomingTransitionsMap.end()) {
    stateTransitionsMapType::iterator stateFound =
      _incomingTransitionsMap.find(state);
    return stateFound->second;
  } else {
    llvm::errs() << "\n ERROR : Could not find state in map";
    exit(0);
  }
}

bool GlobalSuspensionAutomata::isNotifyCall(const CFGStmt * cs)
{

  Stmt *s = const_cast < Stmt * >(cs->getStmt());
  CXXMemberCallExpr *m = dyn_cast < CXXMemberCallExpr > (s);

  if (!m) {
    return false;;
  }

  if (m->getDirectCallee()->getNameInfo().getAsString() == string("notify")) {
    return true;
  }

  return false;
}

string GlobalSuspensionAutomata::getArgumentName(Expr * arg)
{

  LangOptions LangOpts;
  LangOpts.CPlusPlus = true;
  PrintingPolicy Policy(LangOpts);

  string TypeS;
  llvm::raw_string_ostream s(TypeS);

  arg->printPretty(s, 0, Policy);
  return s.str();
}

string GlobalSuspensionAutomata::getNotifyEventName(const CFGStmt * cs)
{

  Stmt *s = const_cast < Stmt * >(cs->getStmt());

  CXXMemberCallExpr *ce = dyn_cast < CXXMemberCallExpr > (s);

  MemberExpr *m = dyn_cast < MemberExpr > (ce->getCallee());

  return getArgumentName(m->getBase()->IgnoreImpCasts());
}

void GlobalSuspensionAutomata::updateEventNotificationTime(Transition * tr)
{

  vector < SusCFG * >codeBlocks = tr->returnCodeBlocks();
  string eventName;
  for (int i = 0; i < codeBlocks.size(); i++) {
    SusCFG *susCFGBlock = codeBlocks.at(i);
    CFGBlock *cfgBlock = susCFGBlock->getBlock();
    for (CFGBlock::iterator it = cfgBlock->begin(), eit = cfgBlock->end();
         it != eit; it++) {
      if (Optional < CFGStmt > cs = it->getAs < CFGStmt > ()) {
        const CFGStmt *s = (CFGStmt const *) &cs;

        if (isNotifyCall(s)) {

          eventName = getNotifyEventName(s);
          float simCycles = 0.0;
          float deltaTime = 0.0;

          if (_transitionTimeMap.find(tr) != _transitionTimeMap.end()) {
            transitionTimeMapType::iterator transitionTimeFound =
              _transitionTimeMap.find(tr);
            _eventNotificationTimeMap.
              insert(eventNotificationTimePairType
                     (eventName, transitionTimeFound->second));
          } else {
            llvm::errs() << "\n ERROR : Cannot find transition time";
          }
        }
      }
    }
  }
}

bool GlobalSuspensionAutomata::updateTransitionTime(Transition * tr)
{

  State *state = tr->returnInitialState();

  if (state->isInitial()) {
    _transitionTimeMap.insert(transitionTimePairType(tr, timePairType(0.0, 0)));
    return true;
  } else if (state->isDelta()) {
    vector < Transition * >transitionVec = getIncomingTransitions(state);
    float simTime = 0.0;
    int deltaTime = 0;

    for (int i = 0; i < transitionVec.size(); i++) {
      if (_transitionTimeMap.find(transitionVec.at(i)) !=
          _transitionTimeMap.end()) {
        transitionTimeMapType::iterator transitionFound =
          _transitionTimeMap.find(transitionVec.at(i));
        if (simTime < transitionFound->second.first) {
          simTime = transitionFound->second.first;
          deltaTime = transitionFound->second.second + 1;
        }
      }
    }
    _transitionTimeMap.
      insert(transitionTimePairType(tr, timePairType(simTime, deltaTime)));
    return true;
  }

  else if (state->isTimed()) {
    vector < Transition * >transitionVec = getIncomingTransitions(state);
    float simTime = 0.0;
    int deltaTime = 0;

    for (int i = 0; i < transitionVec.size(); i++) {
      if (_transitionTimeMap.find(transitionVec.at(i)) !=
          _transitionTimeMap.end()) {
        transitionTimeMapType::iterator transitionFound =
          _transitionTimeMap.find(transitionVec.at(i));
        if (simTime <= transitionFound->second.first) {
          simTime = transitionFound->second.first + state->getSimTime();
          deltaTime = transitionFound->second.second;
        }
      }
    }
    _transitionTimeMap.
      insert(transitionTimePairType(tr, timePairType(simTime, deltaTime)));
    return true;
  } else if (state->isEvent()) {

    if (_eventNotificationTimeMap.find(state->getEventName()) ==
        _eventNotificationTimeMap.end()) {
      return false;

    } else {
      eventNotificationTimeMapType::iterator eventFound =
        _eventNotificationTimeMap.find(state->getEventName());
      _transitionTimeMap.insert(transitionTimePairType(tr, eventFound->second));
      return true;
    }
  } else {
    llvm::errs() << "\n ERROR : Unknown state type";
    return false;
  }
  return true;
}

void GlobalSuspensionAutomata::genGSauto()
{
 /*	
  llvm::errs() << "\n Global Sauto\n";
  llvm::errs() << "\n@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n";
  for (int i = 0; i < _globalSauto.size(); i++) {
    _globalSauto.at(i)->dump(_os);
  }

  llvm::errs() << "\n@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n";
  */
  deque < Transition * >transitionQueue;
  State *initialState;
  for (int i = 0; i < _globalSauto.size(); i++) {
    Transition *t = _globalSauto.at(i);
    if (t->returnInitialState()->isInitial()) {
      initialState = t->returnInitialState();
      break;
    }
  }

  vector < Transition * >initialTransitions =
    getOutgoingTransitions(initialState);

  for (int i = 0; i < initialTransitions.size(); i++) {
    transitionQueue.push_back(initialTransitions.at(i));
  }

  while (transitionQueue.size() != 0) {

    Transition *tr = transitionQueue.front();

    if (updateTransitionTime(tr)) {
      transitionQueue.pop_front();
      _visitTransitionMap.erase(tr);
      _visitTransitionMap.insert(visitTransitionPairType(tr, true));
    } else {
      transitionQueue.pop_front();
      transitionQueue.push_back(tr);
      _visitTransitionMap.insert(visitTransitionPairType(tr, true));
    }
    updateEventNotificationTime(tr);
    vector < Transition * >outgoingTransitions =
      getOutgoingTransitions(tr->returnFinalState());

    for (int i = 0; i < outgoingTransitions.size(); i++) {
      if (_visitTransitionMap[outgoingTransitions.at(i)] == false) {
        transitionQueue.push_back(outgoingTransitions.at(i));
      }
    }
  }
}

void GlobalSuspensionAutomata::dump()
{
  for (instanceFunctionSautoMapType::iterator it =
       _instanceFunctionSautoMap.begin(), eit = _instanceFunctionSautoMap.end();
       it != eit; it++) {
    _os << "\n ############################################\n";
    _os << "\n Instance Name : " << it->first.first;
    _os << "\n Function Name : " << it->first.first->getName();

    vector < Transition * >sauto = it->second;
    for (int i = 0; i < sauto.size(); i++) {
			_os<<"\n Transition : " <<sauto.at(i);
      sauto.at(i)->dump(_os);
      // Scheduled time-stamps for execution
      if (_transitionTimeMap.find(sauto.at(i)) != _transitionTimeMap.end()) {
        transitionTimeMapType::iterator transitionTimeFound =
          _transitionTimeMap.find(sauto.at(i));
        _os << "\n Sim Cycles : " << transitionTimeFound->second.
          first << " Delta cycles : " << transitionTimeFound->second.second;
      }
    }
  }
	for (commonTimeDPMapType::iterator cit = _commonTimeDPMap.begin(), cite = _commonTimeDPMap.end(); cit != cite;
		   cit++) {
  	timePairType timePair = cit->first;
		vector<SusCFG*> susCFGList = cit->second;
		_os <<"\n Time Pair : " <<timePair.first<<" " <<timePair.second;
		_os <<"\n SusCFG* DP : \n";
    for (int j = 0; j<susCFGList.size(); j++) {
    	if (_susCFGBlockGPUMacroMap.find(susCFGList.at(j)) != _susCFGBlockGPUMacroMap.end()) {
      // Found a DP
	    _os <<" "<<susCFGList.at(j)->getBlockID(); 
	    susCFGBlockGPUMacroMapType::iterator DPCodeBlockFound = _susCFGBlockGPUMacroMap.find(susCFGList.at(j));
	    GPUMacro *gpuMacro = DPCodeBlockFound->second;
	    gpuMacro->dump(_os);
    	}			
			if (susCFGList.at(j)->isGPUFit()) {
				_os <<"\n SusCFG Block : " <<susCFGList.at(j)->getBlockID()<<" is marked for GPU execution\n";
			}			
  	}
  }
}


