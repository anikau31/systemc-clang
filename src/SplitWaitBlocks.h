#ifndef _SPLIT_WAIT_BLOCKS_H_
#define _SPLIT_WAIT_BLOCKS_H_

#include "clang/AST/DeclCXX.h"
#include "llvm/Support/raw_ostream.h"
#include "clang/Analysis/CFG.h"
#include "clang/Analysis/CFGStmtMap.h"
#include "clang/AST/ParentMap.h"
#include <map>
#include "FindVariable.h"
#include "EntryFunctionContainer.h"
#include "automata.h"
namespace scpar {
  
  using namespace clang;
  using namespace std;

  class WaitBlock {
  public:
    typedef vector<Optional <CFGStmt> > constCFGStmtListType;
    
    WaitBlock(raw_ostream& );
    WaitBlock(const WaitBlock& );
    ~WaitBlock();
    
    void addPre(Optional <CFGStmt>);
    void addPre(vector< Optional <CFGStmt> > &);    
    void setWaitStmt(Optional<CFGStmt> );
    void clear();    
    
		vector<Optional<CFGStmt> > getPreStmt();    
    Optional <CFGStmt> getWaitStmt();
		
    void dump(raw_ostream& os, int tabn =0);
  
	private:
    Optional <CFGStmt> _waitStmt;
    vector<Optional <CFGStmt> > _preStmt;		
  };

  //////////////////////////////////////////////////////////////////////////////
  class CFGBlockWaitInfo {  
  public:
    typedef vector< WaitBlock* > waitBlockListType;

    CFGBlockWaitInfo(CFGBlock*);
    CFGBlockWaitInfo(const CFGBlockWaitInfo& );
    ~CFGBlockWaitInfo();

    // Accessors.
    void addWaitBlock(vector<WaitBlock*>& );    
    void addWaitBlock(WaitBlock* );

    vector<Optional <CFGStmt> > accessPreStmt();    
    Optional <CFGStmt> accessWaitStmt();
    vector<WaitBlock*> getWaitBlocks();
  
    void dump(raw_ostream& os, int tabn = 0);
	private:
    // Mapping of CFGBlock id to WaitInfos
    waitBlockListType  _waitPos;
    CFGBlock* _cfgBlock;
  };

//////////////////////////////////////////////////////////////////////////////

  class SplitWaitBlocks {
  public:
		typedef pair <EntryFunctionContainer *, CFG*> entryFunctionCFGPairType;
		typedef map <EntryFunctionContainer *, CFG*> entryFunctionCFGMapType;
    
		typedef pair <const CFGBlock*, CFGBlockWaitInfo* > waitInfoPairType;
    typedef map <const CFGBlock*, CFGBlockWaitInfo* > waitInfoMapType;
    
		typedef pair <CFGBlock*, CFGBlock*> waitPair;
    
		typedef map <int, CFGBlock* >waitBlockMap;
    typedef pair<int, CFGBlock*> waitBlockPair;
		
		typedef vector<WaitBlock*> waitBlocksVector;

		typedef pair<EntryFunctionContainer*, waitBlocksVector> SusCFGPairType;
		typedef map<EntryFunctionContainer*, waitBlocksVector> SusCFGMapType;

		typedef pair<int, WaitBlock*> segIDWaitBlockPairType;
		typedef map<int, WaitBlock*> segIDWaitBlockMapType;
		
		typedef pair <bool, string> varTypePair;
		typedef pair<vector<varTypePair>, vector<varTypePair> > accessVectorPairType;
		typedef pair <int, accessVectorPairType> segIDVarPairType;
		typedef map <int, accessVectorPairType> segIDVarMapType;

		typedef pair <EntryFunctionContainer*, vector<int> > entryFunctionSegIDPairType;
		typedef map <EntryFunctionContainer*, vector<int> > entryFunctionSegIDMapType;
		
		typedef pair<EntryFunctionContainer*, Graph*> entryFunctionGraphPairType;
		typedef map<EntryFunctionContainer*, Graph*> entryFunctionGraphMapType;

    SplitWaitBlocks(vector<EntryFunctionContainer*>, ASTContext*, raw_ostream& );
    ~SplitWaitBlocks();

    void findWaitBlocks();
    bool isWaitCall(const CFGStmt* cs );
    void init();    
    void genSauto();
		void extractVarMap();
		void updateVarMap(Stmt*);
   	void updateTimeAdvance();

		waitBlockMap returnWaitBlockMap(); 
		segIDVarMapType returnSegIDVarMap();
		entryFunctionSegIDMapType returnEntryFunctionSegIDMap();
		segIDWaitBlockMapType returnSegIDWaitBlockMap();
		entryFunctionGraphMapType returnEntryFunctionGraphMap();
		
		void dumpVarMap();
		void dump();
		void dumpSusCFG();
  
	private:
  	int _segID;		
    vector <EntryFunctionContainer*> _vef;    
    entryFunctionCFGMapType _entryFunctionCFGMap;
		raw_ostream& _os;
    waitInfoMapType _waitPos;    
    waitPair _waitPair;
    waitBlockMap _waitBlockMap;      
		SusCFGMapType _susCFG;
		segIDWaitBlockMapType _segIDWaitBlockMap;
		entryFunctionGraphMapType _entryFunctionGraphMap;
		entryFunctionSegIDMapType _entryFunctionSegIDMap; 	
		segIDVarMapType _segIDVarMap;		
		vector<varTypePair> _writeVariables;
		vector<varTypePair> _readVariables;
		ASTContext *_a;
	}; // End class SplitWaitBlocks
    
} // End namespace scpar


#endif
