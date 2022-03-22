#ifndef _HDLTHREAD_H
#define _HDLTHREAD_H

// clang-format off

#include "SystemCClang.h"
#include "clang/Analysis/CFG.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/Debug.h"
#include "llvm/ADT/StringRef.h"
#include "hNode.h"
#include "HDLBody.h"
#include "SplitCFG.h"

// clang-format on

using namespace clang;
using namespace systemc_clang;
using namespace hnode;
using namespace llvm;

namespace systemc_hdl {

  class HDLThread {
  public:
    HDLThread(EntryFunctionContainer *efc, hNodep &h_top, hNodep &h_portsigvarlist,
	      clang::DiagnosticsEngine &diag_engine, const ASTContext &ast_context,
	      hdecl_name_map_t &mod_vname_map, hfunc_name_map_t &allmethods, hNodep h_resetvarinfo);
    virtual ~HDLThread();
    
    hfunc_name_map_t methodecls;  //  methods called in this SC_METHOD or function
    
    clang::DiagnosticsEngine &diag_e;

  private:
  
    hNodep h_ret;   // value returned by each subexpression
    EntryFunctionContainer *efc_;
    hNodep h_top_; // reference to calling hnode pointer
    hNodep hthreadblocksp; // collect the case alternatives here
    hNodep hlocalvarsp; // collect the local vars here

    hdecl_name_map_t thread_vname_map;
    bool add_info;
    hdecl_name_map_t &mod_vname_map_; // reference to module level names

    hNodep h_resetvarinfo_;
    
    HDLBody *xtbodyp;
    hfunc_name_map_t allmethodecls_;
    string state_string;
    string nextstate_string;
    string waitctr_string;
    string nextwaitctr_string;
    string waitnextstate_string; // holds the state# to set when ctr=0
    string savewaitnextstate_string; // signal to hold the waitnextstate
    int numstates;
    bool needwaitswitchcase;

    const string nextstring{"_next_"};
    const string shadowstring{"_main_"};
    const string statestringsymbol{"#"};
    
    inline string NameNext(string &s) {return nextstring+s;} // convention for variable name holding next value of var s
    
    // inline string
    std::unordered_map<std::string, bool> SGVisited; // Split Graph Blocks visited 
    std::unordered_map<unsigned int, int> CFGVisited; // CFG Blocks visited 

    inline bool isBreak(const Stmt* S) {
      return dyn_cast<BreakStmt> (S) != NULL;
    }
    
    // pre-pass over BB to mark subexpressions
    void FindStatements(const CFGBlock &B, std::vector<const Stmt *> &SS);
    void FindStatements(const SplitCFGBlock *B, std::vector<const Stmt *> &SS);
    void MarkStatements(const Stmt *S, llvm::SmallDenseMap<const Stmt*, bool> &Map);
    void CheckVardecls(hNodep &hp, unsigned int cfgblockid);
    void ProcessDeclStmt(const DeclStmt *declstmt, hNodep htmp);
    void ProcessSplitGraphBlock(const SplitCFGBlock *sgb, int state_num, hNodep h_switchcase, SplitCFG &scfg);
    void GenerateStateUpdate(hNodep hstatemethod, hNodep hlocalvarsp);
    void GenerateStateVar(string sname);
    void GenerateWaitCntUpdate(hNodep h_switchcase);
    hNodep GenerateBinop(string opname, string lhs, string rhs, bool rhs_is_literal=true);
    bool IsWaitStmt(hNodep hp);
    void ProcessHWait(hNodep htmp, int nxtstate); // rewrite the hWait into next state update

    util lutil;

    const clang::ASTContext& ast_context_;

    // these functions are no longer used
    void ProcessBB(const CFGBlock &BI);
    void AddThreadMethod(const CFGBlock &BI);
	
  };

}

#endif
