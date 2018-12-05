#include "SuspensionAutomata.h"
#include "FindArgument.h"

#include <algorithm>

using namespace scpar;

SusCFG::SusCFG(CFGBlock * block):_block(block),
_parentCFGBlock(NULL),
_parentSusCFGBlock(NULL), _isWaitBlock(false), _isParentBlock(false), _isGPUFit(false)
{

}

SusCFG::~SusCFG()
{
}

void SusCFG::addParentBlock(CFGBlock * parentBlock)
{
  _parentCFGBlock = parentBlock;
}

void SusCFG::addParentBlock(SusCFG * parentBlock)
{
  _parentSusCFGBlock = parentBlock;
}

void SusCFG::addPredBlocks(SusCFG * block)
{
  _predBlocks.push_back(block);
}

void SusCFG::addSuccBlocks(SusCFG * block)
{
  _succBlocks.push_back(block);
}

void SusCFG::addGPUFit()
{
	_isGPUFit = true;
}

void SusCFG::denyGPUFit()
{
	_isGPUFit = false;
}

void SusCFG::addChildBlockList(SusCFG * block)
{
  _childBlockList.push_back(block);
}

void SusCFG::addChildBlockList(vector < SusCFG * >blockList)
{
  for (unsigned int i = 0; i < blockList.size(); i++) {
    _childBlockList.push_back(blockList.at(i));
  }
}

void SusCFG::setWaitBlock()
{
  _isWaitBlock = true;
}

void SusCFG::setParentBlock()
{
  _isParentBlock = true;
}

void SusCFG::setWaitStmt(Stmt * stmt)
{
  _waitStmt = stmt;
}

vector < SusCFG * >SusCFG::getChildBlockList()
{
  return _childBlockList;
}

vector < SusCFG * >SusCFG::getPredBlocks()
{
  return _predBlocks;
}

vector < SusCFG * >SusCFG::getSuccBlocks()
{
  return _succBlocks;
}

unsigned int SusCFG::getBlockID()
{
  return _block->getBlockID();
}

bool SusCFG::isWaitBlock()
{
  return _isWaitBlock;
}

bool SusCFG::isParentBlock()
{
  return _isParentBlock;
}

bool SusCFG::isGPUFit()
{
	return _isGPUFit;
}

unsigned int SusCFG::getParentBlockID()
{
  if (_parentCFGBlock) {
    return _parentCFGBlock->getBlockID();
  } else {
    return 0;
  }
}

CFGBlock *SusCFG::getBlock()
{
  return _block;
}

CFGBlock *SusCFG::getParentCFGBlock()
{
  return _parentCFGBlock;
}

SusCFG *SusCFG::getParentSusCFGBlock()
{
  return _parentSusCFGBlock;
}

Stmt *SusCFG::getWaitStmt()
{
  return _waitStmt;
}

////////////////////////////////////////////////////////////////////////////////////////
State::State(SusCFG * susCFG, bool isTimed, bool isDelta, bool isInitial, bool isEvent):_susCFGBlock(susCFG),
_isTimed(isTimed),
_isDelta(isDelta), _isInitial(isInitial), _isEvent(isEvent)
{

}

State::~State()
{
  delete _susCFGBlock;
}

bool State::isTimed()
{
  return _isTimed;
}

bool State::isDelta()
{
  return _isDelta;
}

bool State::isEvent()
{
  return _isEvent;
}

bool State::isInitial()
{
  return _isInitial;
}

void State::setInitial()
{
  _isInitial = true;
}

void State::setTimed()
{
  _isTimed = true;
}

void State::setEvent()
{

  _isEvent = true;
}

void State::setDelta()
{

  _isDelta = true;
}

void State::addEventName(string eventName)
{
  _eventName = eventName;
}

void State::addSimTime(float simTime)
{
  _timeInNS = simTime;
}

string State::getEventName()
{
  return _eventName;
}

float State::getSimTime()
{
  return _timeInNS;
}

SusCFG *State::returnSusCFGBlock()
{
  return _susCFGBlock;
}

////////////////////////////////////////////////////////////////////////////////////////
Transition::Transition():_initialState(NULL), _finalState(NULL)
{

}

Transition::~Transition()
{
  delete _initialState;
  delete _finalState;
}

void Transition::addInstanceId(int instanceId) {
	_instanceId = instanceId;
}

void
 Transition::addInitialState(State * state)
{
  _initialState = state;
}

void Transition::addFinalState(State * state)
{
  _finalState = state;
}

void Transition::addCodeBlocks(SusCFG * susCFG)
{
  _codeBlockVector.push_back(susCFG);
}

void Transition::addCodeBlocks(vector < SusCFG * >susCFG)
{
  for (unsigned int i = 0; i < susCFG.size(); i++) {
    _codeBlockVector.push_back(susCFG.at(i));
  }
}

int Transition::returnInstanceId() {
	return _instanceId;
}

State *Transition::returnInitialState()
{
  return _initialState;
}

State *Transition::returnFinalState()
{
  return _finalState;
}

vector < SusCFG * >Transition::returnCodeBlocks()
{
  return _codeBlockVector;
}

void Transition::dump(raw_ostream & os)
{
  os << "\n ####### Transition #######";
  os << "\n Initial State : " << _initialState->returnSusCFGBlock()->
    getBlockID();
  os << "\n Final State : " << _finalState->returnSusCFGBlock()->getBlockID();
  os << "\n Transition Blocks : ";
  for (unsigned int i = 0; i < _codeBlockVector.size(); i++) {
    os << _codeBlockVector.at(i)->getBlockID() << " ";
  }
}

////////////////////////////////////////////////////////////////////////////////////////
SuspensionAutomata::SuspensionAutomata(vector<WaitContainer*> waitCalls, CXXMethodDecl * d, ASTContext * a, raw_ostream & os):_d(d), _a(a), _os(os)
{
  
	for (int i = 0; i<waitCalls.size(); i++) {
		WaitContainer *wc = waitCalls.at(i);
		_waitCalls.push_back(wc->getASTNode());
	}
	//_waitCalls = waitCalls;
}

SuspensionAutomata::~SuspensionAutomata()
{
}

bool SuspensionAutomata::initialize()
{
  const CFG::BuildOptions & b = CFG::BuildOptions();

  _cfg = CFG::buildCFG(cast < Decl > (_d), _d->getBody(), _a, b);

  if (_cfg != NULL) {
    return true;
  }

  return false;
}


bool SuspensionAutomata::isWaitCall(const CFGStmt * cs)
{
  bool f = false;

  Stmt *s = const_cast < Stmt * >(cs->getStmt());

  CXXMemberCallExpr *m = dyn_cast < CXXMemberCallExpr > (s);

  if (!m) {
    return f;
  }
  for (int i = 0; i < _waitCalls.size(); i++) {
    CallExpr *ce = _waitCalls.at(i);
    if (m == ce) {
      f = true;
    }
  }
/*
  for (EntryFunctionContainer::waitContainerListType::iterator it =
      _waitCalls.begin(), eit = _waitCalls.end(); it != eit; ++it) 
    WaitContainer *w = (*it);
    if (m == w->getASTNode()) {
      f = true;
    }
 */
  return f;
}

/*************************************************************************
Description : Locates the suspension points in the CFG and 
creates new CFG blocks that isolates the suspension statements from the code.
*************************************************************************/
void SuspensionAutomata::genSusCFG()
{

  LangOptions LO;

  LO.CPlusPlus = true;
  PrintingPolicy Policy(LO);

  vector < Optional < CFGStmt > >pre;
  vector < unsigned int >waitBlockIDVector;
  vector < const CFGBlock *>CFGBlockVector;;

  typedef map < CFGBlock *, SusCFG * >susCFGBlockMapType;
  typedef pair < CFGBlock *, SusCFG * >susCFGBlockPairType;

  susCFGBlockMapType susCFGBlockMap;

  bool isFirstCFGBlockID = false;

  for (CFG::iterator it = _cfg->end() - 1, eit = _cfg->begin(); it != eit; --it) {

    const CFGBlock *b = *it;
    SusCFG *currBlock;

    if (susCFGBlockMap.find(const_cast < CFGBlock * >(b)) ==
        susCFGBlockMap.end()) {
      currBlock = new SusCFG(const_cast < CFGBlock * >(b));
      susCFGBlockMap.insert(susCFGBlockPairType
                            (const_cast < CFGBlock * >(b), currBlock));
    } else {
      susCFGBlockMapType::iterator susCFGBlockFound =
        susCFGBlockMap.find(const_cast < CFGBlock * >(b));
      currBlock = susCFGBlockFound->second;
    }

    bool foundWait = false;

    vector < CFGBlock * >splitBlocksVector;
    /* 
    _os << "==========================================================\n";
    _os << "Dump CFG Block\n";
    b->dump(_cfg, LO, false);
    */
    unsigned int prevCFGBlockID;

    for (CFGBlock::const_iterator bit = b->begin(), bite = b->end();
         bit != bite; bit++) {
      if (Optional < CFGStmt > cs = bit->getAs < CFGStmt > ()) {
        const CFGStmt *s = (CFGStmt const *) &cs;

        if (isWaitCall(s)) {

          foundWait = true;
          CFGBlock *newSplitBlock = _cfg->createBlock();

          for (unsigned int i = 0; i < pre.size(); i++) {
            newSplitBlock->appendStmt(const_cast <
                                      Stmt *
                                      >(pre.at(i)->getStmt()),
                                      _cfg->getBumpVectorContext());
          }

          CFGBlock *waitBlock = _cfg->createBlock();

          waitBlock->appendStmt(const_cast <
                                Stmt * >(cs->getStmt()),
                                _cfg->getBumpVectorContext());
          waitBlock->setLabel(const_cast < Stmt * >(cs->getStmt()));
          splitBlocksVector.push_back(newSplitBlock);
          splitBlocksVector.push_back(waitBlock);
          pre.clear();

        } else {
          pre.push_back(cs);
        }
      }

    }
    //pre.clear();

    if (foundWait) {

      currBlock->setParentBlock();
      CFGBlock *prev = NULL;
      SusCFG *prevBlock = NULL;

      for (unsigned int i = 0; i < splitBlocksVector.size(); i++) {
        CFGBlock *current = splitBlocksVector.at(i);
        SusCFG *splitBlock =
          new SusCFG(const_cast < CFGBlock * >(splitBlocksVector.at(i)));
        currBlock->addChildBlockList(splitBlock);
        if (current->getLabel()) {
          splitBlock->setWaitBlock();
          splitBlock->setWaitStmt(current->getLabel());
        }
        if (pre.size() != 0) {
          // add trailing statements to post block wait stmt
          CFGBlock *newCFGBlock = _cfg->createBlock();
          for (unsigned int j = 0; j < pre.size(); j++) {
            newCFGBlock->appendStmt(const_cast < Stmt * >(pre.at(j)->getStmt()),
                                    _cfg->getBumpVectorContext());
          }
          //SusCFG * postSusCFGBlock = new SusCFG(newCFGBlock);
          //susCFGBlockMap.insert(susCFGBlockPairType(newCFGBlock, postSusCFGBlock));
          splitBlocksVector.push_back(newCFGBlock);
          pre.clear();
        }
        if (i == 0) {
          for (CFGBlock::const_pred_iterator pit =
               b->pred_begin(), pite = b->pred_end(); pit != pite; pit++) {
            const CFGBlock *pred = *pit;

            if (susCFGBlockMap.find
                (const_cast < CFGBlock * >(pred)) == susCFGBlockMap.end()) {
              SusCFG *tmpBlock = new SusCFG(const_cast < CFGBlock * >(pred));
              susCFGBlockMap.insert(susCFGBlockPairType
                                    (const_cast <
                                     CFGBlock * >(pred), tmpBlock));
              splitBlock->addPredBlocks(tmpBlock);
            } else {
              susCFGBlockMapType::iterator susCFGBlockFound =
                susCFGBlockMap.find(const_cast < CFGBlock * >(pred));
              splitBlock->addPredBlocks(susCFGBlockFound->second);
            }
          }

          splitBlock->addParentBlock(const_cast < CFGBlock * >(b));
          splitBlock->addParentBlock(currBlock);
          prevBlock = splitBlock;
        } else if (i == splitBlocksVector.size() - 1) {
          prev = splitBlocksVector.at(i - 1);
          if (susCFGBlockMap.find(prev) == susCFGBlockMap.end()) {
            SusCFG *tmpBlock = new SusCFG(const_cast < CFGBlock * >(prev));
            susCFGBlockMap.insert(susCFGBlockPairType
                                  (const_cast < CFGBlock * >(prev), tmpBlock));
            splitBlock->addPredBlocks(tmpBlock);
          } else {
            susCFGBlockMapType::iterator susCFGBlockFound =
              susCFGBlockMap.find(const_cast < CFGBlock * >(prev));
            splitBlock->addPredBlocks(susCFGBlockFound->second);
          }
          prevBlock->addSuccBlocks(splitBlock);

          for (CFGBlock::const_succ_iterator sit =
               b->succ_begin(), site = b->succ_end(); sit != site; sit++) {
            const CFGBlock *succ = *sit;

            if (susCFGBlockMap.find
                (const_cast < CFGBlock * >(succ)) == susCFGBlockMap.end()) {
              SusCFG *tmpBlock = new SusCFG(const_cast < CFGBlock * >(succ));
              susCFGBlockMap.insert(susCFGBlockPairType
                                    (const_cast <
                                     CFGBlock * >(succ), tmpBlock));
              splitBlock->addSuccBlocks(tmpBlock);
            } else {
              susCFGBlockMapType::iterator susCFGBlockFound =
                susCFGBlockMap.find(const_cast < CFGBlock * >(succ));
              splitBlock->addSuccBlocks(susCFGBlockFound->second);
            }
          }
        } else {
          prev = splitBlocksVector.at(i - 1);
          if (susCFGBlockMap.find
              (const_cast < CFGBlock * >(prev)) == susCFGBlockMap.end()) {
            SusCFG *tmpBlock = new SusCFG(const_cast < CFGBlock * >(prev));
            susCFGBlockMap.insert(susCFGBlockPairType
                                  (const_cast < CFGBlock * >(prev), tmpBlock));
            splitBlock->addPredBlocks(tmpBlock);
          } else {
            susCFGBlockMapType::iterator susCFGBlockFound =
              susCFGBlockMap.find(const_cast < CFGBlock * >(prev));
            splitBlock->addPredBlocks(susCFGBlockFound->second);
          }
          prevBlock->addSuccBlocks(splitBlock);
          prevBlock = splitBlock;
        }
        _susCFGVector.push_back(splitBlock);
      }
    } else {
      for (CFGBlock::const_pred_iterator pit =
           b->pred_begin(), pite = b->pred_end(); pit != pite; pit++) {
        const CFGBlock *predBlock = *pit;

        if (predBlock) {
          if (susCFGBlockMap.find
              (const_cast < CFGBlock * >(predBlock)) == susCFGBlockMap.end()) {
            SusCFG *tmpBlock = new SusCFG(const_cast < CFGBlock * >(predBlock));
            susCFGBlockMap.insert(susCFGBlockPairType
                                  (const_cast <
                                   CFGBlock * >(predBlock), tmpBlock));
            currBlock->addPredBlocks(tmpBlock);
          } else {
            susCFGBlockMapType::iterator predSusCFGBlockFound =
              susCFGBlockMap.find(const_cast < CFGBlock * >(predBlock));
            currBlock->addPredBlocks(predSusCFGBlockFound->second);
          }
        }
      }
      for (CFGBlock::const_succ_iterator sit =
           b->succ_begin(), site = b->succ_end(); sit != site; sit++) {
        const CFGBlock *succBlock = *sit;
        SusCFG *tmpBlock;

        if (succBlock) {
          if (susCFGBlockMap.find
              (const_cast < CFGBlock * >(succBlock)) == susCFGBlockMap.end()) {
            tmpBlock = new SusCFG(const_cast < CFGBlock * >(succBlock));
            susCFGBlockMap.insert(susCFGBlockPairType
                                  (const_cast <
                                   CFGBlock * >(succBlock), tmpBlock));
            currBlock->addSuccBlocks(tmpBlock);
          } else {
            susCFGBlockMapType::iterator succSusCFGBlockFound =
              susCFGBlockMap.find(const_cast < CFGBlock * >(succBlock));
            currBlock->addSuccBlocks(succSusCFGBlockFound->second);
          }
        }
      }
      _susCFGVector.push_back(currBlock);
    }
  }

}

void SuspensionAutomata::addRemainingBlocks(State *initialState, vector<SusCFG*>& transitionBlocks){
 bool duplicate = false;
 if (_stateCommonCodeBlockMap.find(initialState) != _stateCommonCodeBlockMap.end()) {
		stateCommonCodeBlockMapType::iterator stateFound = _stateCommonCodeBlockMap.find(initialState);
  	checkInsert(stateFound->second, transitionBlocks);
	}
}

vector<SusCFG*> SuspensionAutomata::modifDFS(SusCFG * block, State *initialState)
{
  deque < SusCFG * >traversedBlocks;
  vector < SusCFG * >visitedBlocks;

  traversedBlocks.push_front(block);
  bool isWaitEncounter = false; 
  vector < SusCFG * >transitionBlocks;
  while (traversedBlocks.size() != 0) {
    SusCFG *currentBlock = traversedBlocks.front();

    traversedBlocks.pop_front();
    if (currentBlock->isWaitBlock()) {
      float timeInNs;
      string eventName; 
      isWaitEncounter = true;
      susCFGStateMapType::iterator stateFound =
        susCFGStateMap.find(currentBlock);
      State *finalState = stateFound->second;

      if (isTimedWait(currentBlock->getWaitStmt())) {
        finalState->setTimed();
        timeInNs = getTime(currentBlock->getWaitStmt());
      } else if (isEventWait(currentBlock->getWaitStmt())) {

        finalState->setEvent();
        eventName = getEvent(currentBlock->getWaitStmt());
      } else if (isDeltaWait(currentBlock->getWaitStmt())) {
        finalState->setDelta();
      } else {
        finalState->setInitial();
      }      
            
      Transition *t = new Transition();
      finalState->addEventName(eventName);
      finalState->addSimTime(timeInNs);
      t->addInitialState(initialState);
      t->addFinalState(finalState);
      addRemainingBlocks(initialState, transitionBlocks);
      t->addCodeBlocks(transitionBlocks);

      _transitionVector.push_back(t);
      return transitionBlocks;

    } else {      
      // When adding blocks to traversed blocks, check if the last node prior to the wait call has which branch discovered. 
      // Also need to check terminator block or block 0
      if (susCFGSuccIDMap.find(currentBlock) == susCFGSuccIDMap.end()) {
          // currentBlock is not our concern yet, so insert the 0th successive block
          //_os <<"\n Current Block : " <<currentBlock->getBlockID()<<" not of concern";
        if (currentBlock->getSuccBlocks().at(0)->isParentBlock()) {
          if (!isFound(visitedBlocks, currentBlock->getSuccBlocks().at(0)->getChildBlockList().at(0))) {
              traversedBlocks.push_front(currentBlock->getSuccBlocks().at(0)->getChildBlockList().at(0));
          }
        } 
        else {
          if (!isFound(visitedBlocks, currentBlock->getSuccBlocks().at(0))) {
            if (currentBlock->getSuccBlocks().at(0)->getBlockID() != 0) {
              traversedBlocks.push_front(currentBlock->getSuccBlocks().at(0));
            }
          }
        }  
      }
      else {
       // currentBlock has a previous entry in the map, so take the other succesive block if it exists
       //_os <<"\n Current block : " <<currentBlock->getBlockID()<<" is of concern";
       susCFGSuccIDMapType::iterator susCFGFound = susCFGSuccIDMap.find(currentBlock);
       if (susCFGFound->second == currentBlock->getSuccBlocks().size() - 1) {
         // All the child branches of this node have been discovered. So, there is nothing to discover
         //_os <<"\n Current block : " <<currentBlock->getBlockID()<<" has all children accounted for";
         break;
       }
       else {
         if (currentBlock->getSuccBlocks().at(susCFGFound->second + 1)->isParentBlock()) {
           if (!isFound(visitedBlocks, currentBlock->getSuccBlocks().at(susCFGFound->second + 1)->getChildBlockList().at(0))) {
             traversedBlocks.push_front(currentBlock->getSuccBlocks().at(susCFGFound->second + 1)->
                                   getChildBlockList().at(0));
           }
         } 
         else {
           if (!isFound(visitedBlocks, currentBlock->getSuccBlocks().at(susCFGFound->second + 1))) {
             if (currentBlock->getSuccBlocks().at(susCFGFound->second + 1)->getBlockID() != 0) {
               traversedBlocks.push_front(currentBlock->getSuccBlocks().at(susCFGFound->second + 1));
             }
           }
         }   
       }
      }
      visitedBlocks.push_back(currentBlock);
      transitionBlocks.push_back(currentBlock);
    }
  }
    if (isWaitEncounter == false) {
     // we found a path that does not end in a wait block. So, 
     // it will be in all paths from this initial state to all final wait states
     if(_stateCommonCodeBlockMap.find(initialState) == _stateCommonCodeBlockMap.end()) {
      _stateCommonCodeBlockMap.insert(stateCommonCodeBlockPairType(initialState, transitionBlocks));
     }
     else {
      stateCommonCodeBlockMapType::iterator stateFound = _stateCommonCodeBlockMap.find(initialState);
      vector<SusCFG*> remainingCodeBlocks = stateFound->second;
      checkInsert(transitionBlocks, remainingCodeBlocks);  
     }
    }
    return transitionBlocks;
}

void SuspensionAutomata::checkInsert(vector<SusCFG*> source, vector<SusCFG*>& target) {
 		bool duplicate;
  	for (int i = 0; i<source.size(); i++) {
   	 duplicate = false;
	 	 for (int j = 0; j<target.size(); j++) {
   	   if (source.at(i) == target.at(j)) {
   	   	duplicate = true;
	 	    break;
   	   }
   	 }
  	  if (duplicate == false) {  
   	  	 target.push_back(source.at(i));
   	 	}
 	 	}	
}

void SuspensionAutomata::genSauto()
{
  susCFGVectorType susCFGVector = _susCFGVector;
  susCFGVectorType waitBlocks;
  for (int i = 0; i < susCFGVector.size(); i++) {
    if (susCFGVector.at(i)->isWaitBlock() || i == 0) {
      waitBlocks.push_back(susCFGVector.at(i));
      State *state = new State(susCFGVector.at(i), false, false, false, false);
      if (i == 0) {        
        state->setInitial();
        //_os <<"\n State susblock set to initial : " <<state->returnSusCFGBlock()->getBlockID();
        //_os <<"\n State : " <<state->isTimed()<<" " <<state->isInitial()<<" " <<state->isDelta();
      }
      susCFGStateMap.insert(susCFGStatePairType(susCFGVector.at(i), state));
    }
  }
  
  for (int i = 0; i < waitBlocks.size(); i++) {

    SusCFG *waitBlock = waitBlocks.at(i);
 
    //_os <<"\n Looking at Wait Block : " <<waitBlock->getBlockID();

    susCFGStateMapType::iterator stateFound =susCFGStateMap.find(waitBlocks.at(i));
    State *initialState = stateFound->second;

    vector<SusCFG*> backTrackCodeBlocks;
    SusCFG* lastBlock; 
    susCFGSuccIDMap.clear(); // For each new initial state, start fresh...
    // Left child.. do the same for the right child
    do {
      SusCFG *initialInsertBlock;
      if (waitBlock->getSuccBlocks().at(0)->isParentBlock()) {
       initialInsertBlock = waitBlock->getSuccBlocks().at(0)->getChildBlockList().at(0);
      }
      
      else {
        initialInsertBlock = waitBlock->getSuccBlocks().at(0);
      }
      vector<SusCFG*> transitionCodeBlocks = modifDFS(initialInsertBlock, initialState);
       
      //_os <<"\n Transition Blocks : "; 
      backTrackCodeBlocks.clear();
      for (int j = 0; j<transitionCodeBlocks.size(); j++) {
        backTrackCodeBlocks.push_back(transitionCodeBlocks.at(j));
        //_os <<" "<<transitionCodeBlocks.at(j)->getBlockID();;
      }
      int j;
      for (j = backTrackCodeBlocks.size()-2; j >=0; j--) {      
        if (backTrackCodeBlocks.at(j)->getSuccBlocks().size() > 1) {
         //_os <<"\n Block : " <<backTrackCodeBlocks.at(j)->getBlockID()<<" has more than one successor";
         SusCFG* backBlock = backTrackCodeBlocks.at(j);
         if (backBlock->getSuccBlocks().at(0)->isParentBlock()) {

	  if (backBlock->getSuccBlocks().at(0)->getChildBlockList().at(0) == backTrackCodeBlocks.at(j+1)) {
           //_os <<"\n Block : " <<backBlock->getBlockID()<<" used the first successor";
           susCFGSuccIDMap.insert(susCFGSuccIDPairType(backBlock, 0));
           //_os <<"\n Map value : " <<susCFGSuccIDMap[backBlock];
           break;
          }
         }
         else if (backBlock->getSuccBlocks().at(1)->isParentBlock()) {

          if (backBlock->getSuccBlocks().at(1)->getChildBlockList().at(0) == backTrackCodeBlocks.at(j+1)) {
           //_os <<"\n Block : " <<backBlock->getBlockID()<<" used the second successor";
           susCFGSuccIDMap.erase(backBlock);
           susCFGSuccIDMap.insert(susCFGSuccIDPairType(backBlock, 1));
           break;
          }
         }
                 
	 else if (backBlock->getSuccBlocks().at(0) == backTrackCodeBlocks.at(j+1)) {
           //_os <<"\n Block : " <<backBlock->getBlockID()<<" used the first successor";

           susCFGSuccIDMap.insert(susCFGSuccIDPairType(backBlock, 0));
           break;
         }
         else if (backBlock->getSuccBlocks().at(1) == backTrackCodeBlocks.at(j+1)){
           //_os <<"\n Block : " <<backBlock->getBlockID()<<" used the second successor";

           susCFGSuccIDMap.erase(backBlock);
           susCFGSuccIDMap.insert(susCFGSuccIDPairType(backBlock, 1));
           break;
         }
        } 
      }
      //_os <<"\n J value : " <<j; 
      if (j == -1) {
       if (susCFGSuccIDMap.find(backTrackCodeBlocks.back()) == susCFGSuccIDMap.end()) {       
         susCFGSuccIDMap.insert(susCFGSuccIDPairType(backTrackCodeBlocks.back(), 0));
       }
       else {
        if (susCFGSuccIDMap[backTrackCodeBlocks.back()] == 0) {
         susCFGSuccIDMap.erase(backTrackCodeBlocks.back()); 
         susCFGSuccIDMap.insert(susCFGSuccIDPairType(backTrackCodeBlocks.back(), 1));
        }
        else {
        }
       }
      }


      // delete the blocks from the back to j
      if (backTrackCodeBlocks.size() != 0) { 
       backTrackCodeBlocks.pop_back();
      }
      
    }while(backTrackCodeBlocks.size() != 0);
  }
}

template <typename Node>
bool is_found(Node n1, Node n2)
{
    return n1 == n2;
}

template <template <typename, typename> class Container, typename Node, typename Allocator>
bool generic_isFound(Container<Node, Allocator> &container, Node node)
{
    bool foundBlock = false;

    typename Container<Node, Allocator>::iterator itr = std::find_if(
        container.begin(),
        container.end(),
        std::bind1st(std::ptr_fun<Node, Node, bool>(is_found), node)
    );

    return itr != container.end();
}

// need a utility class and this should be a template function
bool SuspensionAutomata::isFound(vector < SusCFG * >visitedState,
                                 SusCFG * block)
{
    return generic_isFound(visitedState, block);
}

bool SuspensionAutomata::isFound(vector < Transition * >visitedState,
                                 Transition * block)
{
    return generic_isFound(visitedState, block);
}


float SuspensionAutomata::getTime(Stmt * stmt)
{
  float factor;
  if (CXXMemberCallExpr * ce = dyn_cast < CXXMemberCallExpr > (stmt)) {
    if (getArgumentName(ce->getArg(1)) == "SC_FS") {
      factor = 1000000;
    } else if (getArgumentName(ce->getArg(1)) == "SC_PS") {
      factor = 1000;
    } else if (getArgumentName(ce->getArg(1)) == "SC_NS") {
      factor = 1;
    } else if (getArgumentName(ce->getArg(1)) == "SC_US") {
      factor = 0.001;
    } else if (getArgumentName(ce->getArg(1)) == "SC_MS") {
      factor = 0.000001;
    } else if (getArgumentName(ce->getArg(1)) == "SC_SEC") {
      factor = 0.0000000001;
    }
    return (atof(getArgumentName(ce->getArg(0)).c_str()) * factor);
  }
}

string SuspensionAutomata::getEvent(Stmt * stmt)
{
  if (CXXMemberCallExpr * ce = dyn_cast < CXXMemberCallExpr > (stmt)) {
    return (getArgumentName(ce->getArg(0)));
  }
}


bool SuspensionAutomata::isTimedWait(Stmt * stmt)
{

  if (CXXMemberCallExpr * ce = dyn_cast < CXXMemberCallExpr > (stmt)) {
    if (ce->getNumArgs() > 1) {
      return true;
    }
  }
  return false;
}
/*
string SuspensionAutomata::getArgumentName(Expr * arg)
{
  if (arg == NULL)
    return string("NULL");

  clang::LangOptions LangOpts;
  LangOpts.CPlusPlus = true;
  clang::PrintingPolicy Policy(LangOpts);

  string TypeS;

  llvm::raw_string_ostream s(TypeS);

  arg->printPretty(s, 0, Policy);
  //  _os << ", argument: " << s.str() << "\n";
  return s.str();
}
*/

bool SuspensionAutomata::isDeltaWait(Stmt * stmt)
{

  if (CXXMemberCallExpr * ce = dyn_cast < CXXMemberCallExpr > (stmt)) {
    if (ce->getNumArgs() == 1) {
      string eventName = getArgumentName(ce->getArg(0));

      if (eventName == "SC_ZERO_TIME") {
        return true;
      } else {
        return false;
      }
    }
  }
  return false;
}

bool SuspensionAutomata::isEventWait(Stmt * stmt)
{

  if (CXXMemberCallExpr * ce = dyn_cast < CXXMemberCallExpr > (stmt)) {
    if (ce->getNumArgs() == 1) {
      return true;
    }
  }
  return false;
}

SuspensionAutomata::susCFGVectorType SuspensionAutomata::getSusCFG()
{
  return _susCFGVector;
}

SuspensionAutomata::transitionVectorType SuspensionAutomata::getSauto()
{
  return _transitionVector;
}

void SuspensionAutomata::dumpSusCFG()
{

  susCFGVectorType susCFGVector = _susCFGVector;

  for (unsigned int i = 0; i < susCFGVector.size(); i++) {
    _os << "\n Block ID : " << susCFGVector.at(i)->getBlockID();
    _os << "\n Is Wait Block : " << susCFGVector.at(i)->isWaitBlock();
    if (susCFGVector.at(i)->getParentBlockID()) {
      _os << "\n Parent ID : " << susCFGVector.at(i)->getParentBlockID();
      SusCFG *parentBlock = susCFGVector.at(i)->getParentSusCFGBlock();

      _os << "\n Parent Block ID : " << parentBlock->getBlockID();
      _os << "\n Size of Children : " <<
        parentBlock->getChildBlockList().size();
    }
    vector < SusCFG * >predBlocks = susCFGVector.at(i)->getPredBlocks();
    vector < SusCFG * >succBlocks = susCFGVector.at(i)->getSuccBlocks();
    _os << "\n Predecessor Blocks : ";
    for (unsigned int j = 0; j < predBlocks.size(); j++) {
      if (predBlocks.at(j)) {
        _os << predBlocks.at(j)->getBlockID() << " ";
      }
    }
    _os << "\n Successor Blocks : ";
    for (unsigned int j = 0; j < succBlocks.size(); j++) {
      if (succBlocks.at(j)) {
        _os << succBlocks.at(j)->getBlockID() << " ";
      }
    }
  }

}

void SuspensionAutomata::dumpSauto()
{

  vector < Transition * >transitionVector = _transitionVector;
  _os << "\n Size of transitionVector : " << transitionVector.size();
  for (unsigned int i = 0; i < transitionVector.size(); i++) {
    Transition *t = transitionVector.at(i);
    t->dump(_os);
  }
}

//////////////////////////////////////////////////////////////////////////////
