#include "SplitWaitBlocks.h"

using namespace scpar;

SplitWaitBlocks::SplitWaitBlocks(vector<EntryFunctionContainer*> ve, ASTContext* a, raw_ostream& os)
  : _vef(ve)
	,	_a(a)  
  , _os(os)
	, _segID(0){
  
}

SplitWaitBlocks::~SplitWaitBlocks() {

  for (SplitWaitBlocks::waitInfoMapType::iterator it = _waitPos.begin(), eit = _waitPos.end();
       it != eit;
       it++) {
    delete it->second;
  }

}

void SplitWaitBlocks::init() {

	for (int i = 0; i<_vef.size(); i++) {		
		const CFG::BuildOptions &b = CFG::BuildOptions();
		EntryFunctionContainer *e = _vef.at(i);
		CXXMethodDecl *d = e->getEntryMethod();
		CFG* _cfg = CFG::buildCFG(cast<Decl>(d), d->getBody(), _a, b);
		_entryFunctionCFGMap.insert(entryFunctionCFGPairType(e, _cfg));
	}
}

bool SplitWaitBlocks::isWaitCall(const CFGStmt* cs ) {
  bool f = false;
  for (int i = 0; i<_vef.size(); i++) {
		EntryFunctionContainer *_ef = _vef.at(i);
		EntryFunctionContainer::waitContainerListType waitCalls = _ef->getWaitCalls();

  	Stmt* s = const_cast<Stmt*>(cs->getStmt());

  	if (!s) {
    //    s->dump(); 
  	}
  	CXXMemberCallExpr* m = dyn_cast<CXXMemberCallExpr>(s);
  	if (!m) {
    	return f;
  	}
  
  // _os << "**************** CHECK FOR WAIT ************ : " << waitCalls.size() << "\n";
  	for (EntryFunctionContainer::waitContainerListType::iterator it = waitCalls.begin(), eit = waitCalls.end();
       it != eit;
       ++it ) {
    	WaitContainer* w = (*it);
    //    _os << "m: " << m << ", w: " << w->getASTNode() << "\n";
			if (m == w->getASTNode()) {
      //    _os << "FOUND THE WAIT\n";
      //  m->dump();
      f = true;
    	}
  	}  	
	}
	return f;
}

void SplitWaitBlocks::findWaitBlocks() {

  LangOptions LO;
  LO.CPlusPlus = true;
  PrintingPolicy Policy(LO);

  int state =0; 
  vector<Optional <CFGStmt> > pre;

	for (entryFunctionCFGMapType::iterator it = _entryFunctionCFGMap.begin(), eit = _entryFunctionCFGMap.end();
	it != eit;
	it++) 
	{ 	
		EntryFunctionContainer *e = it->first;
		CFG *_cfg = it->second;
		CXXMethodDecl* d = e->getEntryMethod();

  	vector<int> segIDVector;

    vector<WaitBlock* > waitBlocks;

  	for (CFG::iterator it = _cfg->end()-1, eit = _cfg->begin(); 
    	   it != eit;
    	   --it) {
      
    	const CFGBlock* b = *it;
  		vector<WaitBlock*> CFGBlockWaitBlocks; 
    	_os << "==========================================================\n";
    	_os << "Dump CFG Block\n";
    	b->dump(_cfg, LO, true);
    

   	for (CFGBlock::const_iterator bit = b->begin(), bite = b->end();
         bit != bite;
	 			++bit) {
      	if (Optional <CFGStmt> cs = bit->getAs<CFGStmt>()) {

					const CFGStmt *s = (CFGStmt const*) &cs;
						
					if (isWaitCall(s)) {							  					  				
						WaitBlock* wb = new WaitBlock(_os);
	  				wb->setWaitStmt(cs);
	  				wb->addPre(pre);
	 					waitBlocks.push_back(wb);
						CFGBlockWaitBlocks.push_back(wb);
	  				_segIDWaitBlockMap.insert(segIDWaitBlockPairType(_segID, wb));
						segIDVector.push_back(_segID);
						_segID++;
						pre.clear();	  				
						continue;
					} // End if					
					pre.push_back(cs);
					//Add post for current wait block
      	} // End if (const ...)
    	} // End for CFGBlock
			if(CFGBlockWaitBlocks.size() != 0) {
				CFGBlockWaitInfo *cfgBlockWaitInfo = new CFGBlockWaitInfo(const_cast<CFGBlock*>(b));
				cfgBlockWaitInfo->addWaitBlock(CFGBlockWaitBlocks);
				
				_waitPos.insert(waitInfoPairType(const_cast<CFGBlock*>(b), cfgBlockWaitInfo));
				/* Ugly name waitPos. Need to change this later..... */
			}
		}
		if(pre.size() != 0) {
			WaitBlock *start = waitBlocks.front();
			start->addPre(pre);
		}
		_susCFG.insert(SusCFGPairType(e, waitBlocks));
		_entryFunctionSegIDMap.insert(entryFunctionSegIDPairType(e, segIDVector));
	}
}

void SplitWaitBlocks::extractVarMap() {

	for (SplitWaitBlocks::segIDWaitBlockMapType::iterator it = _segIDWaitBlockMap.begin(), eit = _segIDWaitBlockMap.end();
	it != eit;
	it++) {
		WaitBlock *wb = it->second;
		vector<Optional <CFGStmt> > preStmtVector = wb->getPreStmt();		
		for (int i = 0; i<preStmtVector.size(); i++) {
			Optional <CFGStmt> cfgStmt = preStmtVector.at(i);
			Stmt* stmt = const_cast<Stmt*>(cfgStmt->getStmt());

			updateVarMap(stmt);
		}
		_segIDVarMap.insert(segIDVarPairType(it->first, accessVectorPairType(_readVariables, _writeVariables)));
		_readVariables.clear();
		_writeVariables.clear();
	}
}

void SplitWaitBlocks::updateVarMap(Stmt *stmt) {
	bool foundVar = false;

	if(CXXMemberCallExpr *ce = dyn_cast<CXXMemberCallExpr> (stmt)) {
		if(ce->getMethodDecl()->getNameAsString() == "write") {
			FindVariable v(ce->getCallee()->IgnoreImpCasts());
			if(v.getArgumentName() != "NONE") {
				for (int i = 0; i<_writeVariables.size(); i++) {
					if(v.getArgumentName() == _writeVariables.at(i).second) {
						foundVar = true;
						break;
					}
				}
				if(!foundVar) {
					_writeVariables.push_back(varTypePair(v.getAccessType(), v.getArgumentName()));
				}
			}
		}
	}
	if(BinaryOperator *bo = dyn_cast<BinaryOperator>(stmt)) {
	//	_os <<"\n BINARY OP DUMP\n";
	//	bo->dump();
		
		FindVariable lv(bo->getLHS()->IgnoreImpCasts());
		if(lv.getArgumentName() != "NONE") {
			if(bo->isComparisonOp()) {
				for (int i = 0; i<_readVariables.size(); i++) {
					if(lv.getArgumentName() == _readVariables.at(i).second) {
						foundVar = true;
						break;
					}
				}
				if(!foundVar) {					
					_readVariables.push_back(varTypePair(lv.getAccessType(), lv.getArgumentName()));
				}
			}
			else {
				for (int i = 0; i<_writeVariables.size(); i++) {
					if(lv.getArgumentName() == _writeVariables.at(i).second) {
						foundVar = true;
						break;
					}
				}
				if(!foundVar) {				
					_writeVariables.push_back(varTypePair(lv.getAccessType(), lv.getArgumentName()));
				}
			}
		}
		FindVariable rv(bo->getRHS()->IgnoreImpCasts());
		if(rv.getArgumentName() != "NONE") {
			for (int i = 0; i<_readVariables.size(); i++) {
				if(rv.getArgumentName() == _readVariables.at(i).second) {
					foundVar = true;
					break;
				}
			}
			if(!foundVar){				
				_readVariables.push_back(varTypePair(rv.getAccessType(), rv.getArgumentName()));
			}
		}
	}
	
	if(UnaryOperator *uo = dyn_cast<UnaryOperator>(stmt)) {
		FindVariable v(uo->getSubExpr()->IgnoreImpCasts());
		if(v.getArgumentName()!= "NONE") {
			for (int i = 0; i<_writeVariables.size(); i++) {
				if(v.getArgumentName() == _writeVariables.at(i).second) {
					foundVar = true;
					break;
				}				
			}
			if(!foundVar){
				_writeVariables.push_back(varTypePair(v.getAccessType(), v.getArgumentName()));
			}
			foundVar = false;
			for (int i = 0; i<_readVariables.size(); i++) {
				if(v.getArgumentName() == _readVariables.at(i).second) {
					foundVar = true;
					break;
				}				
			}
			if(!foundVar){			
				_readVariables.push_back(varTypePair(v.getAccessType(), v.getArgumentName()));
			}
		}
	}
	
	/* Compound statements */
}

void SplitWaitBlocks::genSauto() {

	LangOptions LO;
	LO.CPlusPlus = true;
	PrintingPolicy Policy(LO);
	
	for (entryFunctionCFGMapType::iterator it = _entryFunctionCFGMap.begin(), eit = _entryFunctionCFGMap.end();
	it != eit;
	it++) {
		EntryFunctionContainer *e = it->first;
		vector<CFGBlock*> cfgBlocksPath;
		CXXMethodDecl *d = e->getEntryMethod();
		CFG *_cfg = it->second;
		bool conditionBlock = false;

		Graph *g = new Graph();
		int stateId = 0;
		g->addNode(0);
			
		if(_entryFunctionSegIDMap.find(e) != _entryFunctionSegIDMap.end()) {
			entryFunctionSegIDMapType::iterator entryFunctionFound = _entryFunctionSegIDMap.find(e);
			
			for (CFG::iterator it = _cfg->end()-1 , eit = _cfg->begin();
			it != eit;
			--it) {
				const CFGBlock *b = *it;
				const CFGBlock *prevB = *(it+1);				
				unsigned int elseBlockID;
				unsigned int parentConditionBlockID;
				const CFGBlock *parentConditionBlock;
				
				if(_waitPos.find(const_cast<CFGBlock*>(b)) != _waitPos.end()) {								 	 CFGBlock *stmtBlock = _cfg->createBlock();
					Optional <CFGStmt> cs;
					for (CFGBlock::const_iterator bit = b->begin(), bite = b->end();
					bit != bite;
					bit++) {
						if(cs = bit->getAs<CFGStmt>()) {
							const CFGStmt *s = (CFGStmt const*) &cs;
							if (isWaitCall(s)) {
								stateId++;
								g->addNode(stateId);							
								g->updateNode(b, stateId);
								cfgBlocksPath.push_back(stmtBlock);
								g->addEdge(stateId-1, stateId, cfgBlocksPath, cs);
								if(!conditionBlock) {
									cfgBlocksPath.clear();
								}
							}
							else {
								stmtBlock->appendStmt(const_cast<Stmt*>(cs->getStmt()), _cfg->getBumpVectorContext());	
							}
						}
					}	
					if(conditionBlock) {
						conditionBlock = false;
						if(b->getBlockID() == elseBlockID) {

							if(g->getNodeID(parentConditionBlock) != 0) {
								unsigned int parentID = g->getNodeID(parentConditionBlock);		
								g->addEdge(parentID, stateId, cfgBlocksPath, cs);
								cfgBlocksPath.clear();
							}
						}
					}
					if((prevB) && prevB->succ_size() > 1) {
						conditionBlock = true;
						CFGBlock::const_succ_iterator elseBlockFound = prevB->succ_end() - 1;	
						const CFGBlock *elseBlock = *elseBlockFound;
						if(elseBlock) {
							elseBlockID = elseBlock->getBlockID();
							parentConditionBlock = prevB;							
						}
					}					
				}
				else {
					// normal cfg block without any wait.					
					cfgBlocksPath.push_back(const_cast<CFGBlock*>(b));	
				}
			}
		}		
		Graph::adjMapType adjMap = g->returnAdjList();
		//g->dumpSauto(_os, 0);
		_entryFunctionGraphMap.insert(entryFunctionGraphPairType(e, g));
	}	
}

SplitWaitBlocks::entryFunctionSegIDMapType SplitWaitBlocks::returnEntryFunctionSegIDMap() {
	return _entryFunctionSegIDMap;
}

SplitWaitBlocks::entryFunctionGraphMapType SplitWaitBlocks::returnEntryFunctionGraphMap() {
	return _entryFunctionGraphMap;
}

SplitWaitBlocks::segIDWaitBlockMapType SplitWaitBlocks::returnSegIDWaitBlockMap() {
	return _segIDWaitBlockMap;
}

SplitWaitBlocks::segIDVarMapType SplitWaitBlocks::returnSegIDVarMap() {
	return _segIDVarMap;
}

SplitWaitBlocks::waitBlockMap SplitWaitBlocks::returnWaitBlockMap() {
	return _waitBlockMap;
}

void SplitWaitBlocks::dumpVarMap() {

	for(SplitWaitBlocks::entryFunctionSegIDMapType::iterator it = _entryFunctionSegIDMap.begin(), 
	eit = _entryFunctionSegIDMap.end();
	it != eit;
	it++) {
		_os <<"\n For entry function : " <<it->first->getName()<<" seg IDs are : \n";
		vector<int> tmp = it->second;
		for (int i = 0 ; i<tmp.size(); i++) {
			_os <<tmp.at(i)<<" ";
		}
	}

	for (SplitWaitBlocks::segIDWaitBlockMapType::iterator it = _segIDWaitBlockMap.begin(), eit = _segIDWaitBlockMap.end(); 
	it != eit;
	it++) {
		_os <<"\n For SEG ID : " <<it->first;
		it->second->dump(_os , 0);
	}

	for (SplitWaitBlocks::segIDVarMapType::iterator it = _segIDVarMap.begin(), eit = _segIDVarMap.end(); 
	it != eit;
	it++) {
		_os <<"\n For SEG ID : " <<it->first;
		accessVectorPairType tmpPair = it->second;
		vector<varTypePair> tmpReadVar = tmpPair.first;
		vector<varTypePair> tmpWriteVar = tmpPair.second;
		_os <<"\n READ VARIABLES \n";
		for (int i = 0; i<tmpReadVar.size(); i++) {
			_os <<"   "<<tmpReadVar.at(i).second<<" "<<tmpReadVar.at(i).first<<" " ;
		}
		_os<<"\n WRITE VARIABLES \n";
		for (int i = 0; i<tmpWriteVar.size(); i++) {
			_os <<"   "<<tmpWriteVar.at(i).second<<" "<<tmpWriteVar.at(i).first<<" ";
		}
	}
}

void SplitWaitBlocks::dumpSusCFG() {
	for (SplitWaitBlocks::SusCFGMapType::iterator it = _susCFG.begin(), eit = _susCFG.end(); 
	it != eit;
	it++) {
		EntryFunctionContainer *entryFunction = it->first;
		_os <<"\n SUS CFG for entry function container : " <<entryFunction->getName();
		waitBlocksVector tmpVector = it->second;
		for (int i = 0; i<tmpVector.size(); i++) {
			WaitBlock * tmpWB = tmpVector.at(i);
			tmpWB->dump(_os , 0);
		}
	}
}

void SplitWaitBlocks::dump(){

  for (SplitWaitBlocks::waitInfoMapType::iterator it = _waitPos.begin(), eit = _waitPos.end();
       it != eit;
       it++) {

    CFGBlockWaitInfo *tmp = it->second;
    tmp->dump(_os);	
						
  }
}

//////////////////////////////////////////////////////////////////////////////
WaitBlock:: WaitBlock(raw_ostream &os)
  : _waitStmt(NULL) {
}

WaitBlock:: WaitBlock(const WaitBlock& from ) {
  _waitStmt = from._waitStmt;
  _preStmt = from._preStmt; 
}

WaitBlock::~WaitBlock() {
  // DO NOT free anything.
  _preStmt.clear();  
}


void WaitBlock::addPre(Optional <CFGStmt> s) {
  _preStmt.push_back(s);
}

void WaitBlock::clear() {
  _preStmt.clear();  
}

void WaitBlock::setWaitStmt(Optional <CFGStmt> s) {
  _waitStmt = s;
}

void WaitBlock::addPre(WaitBlock::constCFGStmtListType& prev) {
  for (WaitBlock::constCFGStmtListType::iterator it = prev.begin(), eit = prev.end();
       it != eit;
       it++) {
    _preStmt.push_back(*it);
  }
}

void WaitBlock::dump(raw_ostream& os, int tabn) {

  LangOptions LO;
  LO.CPlusPlus = true;
  PrintingPolicy Policy(LO);

  os <<"\nDumping pre-statements\n";
  for (std::vector<Optional <CFGStmt> >::iterator it = _preStmt.begin();
       it != _preStmt.end();
       it++) {
    Optional <CFGStmt> g = *it;
    g->getStmt()->printPretty(llvm::errs(), 0, Policy, 0);
    os <<"\n";
  } 
}
vector <Optional <CFGStmt> > WaitBlock::getPreStmt() {
	return (_preStmt);
}

Optional <CFGStmt> WaitBlock::getWaitStmt() {
	return (_waitStmt);
}
/////////////////////////////////////////////////////////////////////////////////////

CFGBlockWaitInfo::CFGBlockWaitInfo(CFGBlock* b)
  : _cfgBlock(b) {

}

CFGBlockWaitInfo::CFGBlockWaitInfo(const CFGBlockWaitInfo& from) {
  _waitPos = from._waitPos;
  _cfgBlock = from._cfgBlock;

}

CFGBlockWaitInfo::~CFGBlockWaitInfo() {
  // DO NOT delete the following
  // - _cfgBlock
  for (CFGBlockWaitInfo::waitBlockListType::iterator it = _waitPos.begin(), eit = _waitPos.end();
       it != eit;
       it++) {
    delete (*it);

  }  
}

void CFGBlockWaitInfo::addWaitBlock(vector<WaitBlock*>& bv) {
  for (unsigned int i = 0;
       i < bv.size();
       i++) {
    _waitPos.push_back(bv[i]);
  }
}

void CFGBlockWaitInfo::addWaitBlock(WaitBlock* b) {
  _waitPos.push_back(b);
}

void CFGBlockWaitInfo::dump(raw_ostream& os, int tabn) {

  os << "CFGBlockWaitInfo " << _cfgBlock->getBlockID() << " , num of waitblocks: " << _waitPos.size() << " \n";
  for (CFGBlockWaitInfo::waitBlockListType::iterator it = _waitPos.begin(), eit = _waitPos.end();
       it != eit;
       it++) {
    (*it)->dump(os, tabn);

  }
}

vector<Optional <CFGStmt> > CFGBlockWaitInfo::accessPreStmt() {
	for (CFGBlockWaitInfo::waitBlockListType::iterator it = _waitPos.begin(),
	eit = _waitPos.end();
	it != eit;
	it++) {
		return((*it)->getPreStmt());
	}
  return vector<Optional <CFGStmt> >();
}

Optional <CFGStmt> CFGBlockWaitInfo::accessWaitStmt() {
	for (CFGBlockWaitInfo::waitBlockListType::iterator it = _waitPos.begin(),
	eit = _waitPos.end();
	it != eit;
	it++) {
		return((*it)->getWaitStmt());
	}
  //return NULL;
}

vector <WaitBlock*> CFGBlockWaitInfo::getWaitBlocks(){
	return _waitPos;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

/* Generating automaton */
/*
Graph SplitWaitBlocks::testgraph() {

  Graph g;

  bool conditionFound;
  bool duplicateGuard;
  vector<CFGBlock*> pre;
  vector <CFGBlock*> post;
  vector <CFGBlock*>guard;
  vector <CFGBlock *>seenBlocks; //tmp variable
 
  for (SplitWaitBlocks::pathWaitMap::iterator it = _pathMap.begin(), eit = _pathMap.end();
	it != eit;
	it++) {

		conditionFound = false;
		duplicateGuard = false;

		//getting suspension statements
		SplitWaitBlocks::waitPair tmp = it->first;
		SplitWaitBlocks::path paths = it->second;
		Node *n1;

		if(tmp.first->getBlockID() == _cfg->getEntry().getBlockID()){

			n1 = g.addNode(-1);
		}
		else{
			n1 = g.addNode(tmp.first->getBlockID());
			pre.push_back(paths.at(0));

		}
		Node *n2 = g.addNode(tmp.second->getBlockID());
		
		for (unsigned int j = 1; j<paths.size(); j++) {		
		
			if(conditionFound == true) {
				if(_waitBlockMap.find(paths.at(j)->getBlockID()) == _waitBlockMap.end()){	
					post.push_back(paths.at(j));
				
				}
			}

			if((paths.at(j)->succ_size() > 1) && (paths.at(j+1) == paths.back())){ 	//condition block
				conditionFound = true;
				guard.push_back(paths.at(j));
				seenBlocks.push_back(paths.at(j));
			}
			if(conditionFound == false){
				if(_waitBlockMap.find(paths.at(j)->getBlockID()) == _waitBlockMap.end()) {
// deleted starts here
					for (int i = 0; i<seenBlocks.size();i++) {
						if(seenBlocks.at(i)->getBlockID() == paths.at(j)->getBlockID()) {
							duplicateGuard = true;
							break;	
						}
					}
// deleted ends here 
			//		if(duplicateGuard == false){
						pre.push_back(paths.at(j));
				//	}
				
				}		
			}
//		duplicateGuard = false;
		}
	Edge *e = g.addEdge(n1, n2, pre, guard, post, tmp.second);
	pre.clear();
	post.clear();
	guard.clear();
	
			
	}

//	g.dumpSauto(_os, 0);
	return g;
}
*/
///////////////////////////////////////////////////////////////////////////////////////////////

