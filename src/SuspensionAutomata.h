#ifndef _SUSPENSION_AUTOMATA_H_
#define _SUSPENSION_AUTOMATA_H_

#include "clang/AST/DeclCXX.h"
#include "llvm/Support/raw_ostream.h"
#include "clang/Analysis/CFG.h"
#include "clang/Analysis/CFGStmtMap.h"
#include "clang/AST/ParentMap.h"
#include <map>
#include "Utility.h"
#include "WaitContainer.h"
#include "Automata.h"
namespace scpar {

  using namespace clang;
  using namespace std;

  class SusCFG {
  public:

    SusCFG(CFGBlock *);
    ~SusCFG();
    void addPredBlocks(SusCFG *);
    void addSuccBlocks(SusCFG *);
    void addParentBlock(CFGBlock *);
    void addParentBlock(SusCFG *);
    void setWaitBlock();
    void setWaitStmt(Stmt *);
    void setParentBlock();
    void addChildBlockList(SusCFG *);
    void addChildBlockList(vector < SusCFG * >);
		void addGPUFit();
		void denyGPUFit();

    bool isWaitBlock();
    bool isParentBlock();
    bool isParentWaitBlock(); 
		bool isGPUFit();

    unsigned int getBlockID();
    unsigned int getParentBlockID();
    vector < SusCFG * >getChildBlockList();
    vector < SusCFG * >getPredBlocks();
    vector < SusCFG * >getSuccBlocks();
    CFGBlock *getBlock();
    CFGBlock *getParentCFGBlock();
    SusCFG *getParentSusCFGBlock();
    Stmt *getWaitStmt();
  private:
    CFGBlock * _block;
    CFGBlock *_parentCFGBlock;
    SusCFG *_parentSusCFGBlock;
    vector < SusCFG * >_predBlocks;
    vector < SusCFG * >_succBlocks;
    // need to add a isTranslateCUDA field here
		bool _isWaitBlock;
    bool _isParentBlock;
    bool _isGPUFit;
		Stmt *_waitStmt;
     vector < SusCFG * >_childBlockList;
  };
  ////////////////////////////////////////////////////////////////////////////////////////
  class State {
  public:
    State(SusCFG *, bool, bool, bool, bool);
    ~State();

    bool isTimed();
    bool isDelta();
    bool isInitial();
    bool isEvent();
    void addEventName(string);
    void addSimTime(float);
    void setTimed();
    void setDelta();
    void setEvent();
    void setInitial();

    string getEventName();
    float getSimTime();
    SusCFG *returnSusCFGBlock();
  private:
    SusCFG * _susCFGBlock;
    bool _isTimed;
    bool _isDelta;
    bool _isInitial;
    bool _isEvent;
    string _eventName;
    float _timeInNS;
  };
  ////////////////////////////////////////////////////////////////////////////////////////
  class Transition {
  public:
    Transition();
    ~Transition();

    void addInitialState(State *);
    void addFinalState(State *);
    void addCodeBlocks(SusCFG *);
    void addCodeBlocks(vector < SusCFG * >);
		void addInstanceId(int);

    State *returnInitialState();
    State *returnFinalState();
    vector < SusCFG * >returnCodeBlocks();
		int returnInstanceId();

    void dump(raw_ostream &);
  private:
		int _instanceId;
    vector < SusCFG * >_codeBlockVector;
    State *_initialState;
    State *_finalState;
  };

  //////////////////////////////////////////////////////////////////////////////

  class SuspensionAutomata : public Utility{
  public:

    typedef vector < SusCFG * >susCFGVectorType;
    typedef vector <Transition *> transitionVectorType;
    
    typedef pair<SusCFG*, int> susCFGSuccIDPairType;
    typedef map<SusCFG*, int> susCFGSuccIDMapType;
    
    typedef pair < SusCFG *, State * >susCFGStatePairType;
    typedef map < SusCFG *, State * >susCFGStateMapType;
	
    typedef pair<State*, vector<SusCFG*> > stateCommonCodeBlockPairType;
    typedef map<State *, vector<SusCFG*> > stateCommonCodeBlockMapType;

    SuspensionAutomata(vector<WaitContainer*>, CXXMethodDecl *, ASTContext *,
                        raw_ostream &);
    ~SuspensionAutomata();
    void addRemainingBlocks(State*, vector<SusCFG*>&);
    void checkInsert(vector<SusCFG*>, vector<SusCFG*>&);
    bool isFound(vector < SusCFG * >, SusCFG *);  
    bool isFound(vector < Transition * >, Transition *);
    bool initialize();
    void genSusCFG();
    void genSauto();
    vector<SusCFG*> modifDFS(SusCFG*, State*);
    bool isWaitCall(const CFGStmt * cs);
    bool isTimedWait(Stmt * stmt);
    bool isDeltaWait(Stmt * stmt);
    bool isEventWait(Stmt * stmt);
    float getTime(Stmt *stmt);
    string getEvent(Stmt *stmt);
    //string getArgumentName(Expr * arg);
    void addEvent(string);
    void addSimTime(float);
    susCFGVectorType getSusCFG();
    transitionVectorType getSauto();

    void dumpSusCFG();
    void dumpSauto();
  private:
    CXXMethodDecl *_d;
    vector<CallExpr*> _waitCalls;
    susCFGSuccIDMapType susCFGSuccIDMap;
    susCFGStateMapType susCFGStateMap;
    unique_ptr<CFG> _cfg;
    ASTContext *_a;
    raw_ostream & _os;
    susCFGVectorType _susCFGVector;
    transitionVectorType _transitionVector;
    stateCommonCodeBlockMapType _stateCommonCodeBlockMap;
  };                            // End class SplitWaitBlocks
}                               // End namespace scpar
#endif
