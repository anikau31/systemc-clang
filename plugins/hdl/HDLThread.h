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
  class SCBBlock;

  class HDLThread {
  public:
    HDLThread(CXXMethodDecl * emd, hNodep &h_top, clang::DiagnosticsEngine &diag_engine, const ASTContext &ast_context,  hdecl_name_map_t &mod_vname_map);
    virtual ~HDLThread();
    
    hfunc_name_map_t methodecls;  //  methods called in this SC_METHOD or function

    clang::DiagnosticsEngine &diag_e;

  private:
  
    hNodep h_ret;   // value returned by each subexpression

    hNodep h_top_; // reference to calling hnode pointer
    hNodep hthreadblocksp; // collect the methods here
    hNodep hlocalvarsp; // collect the local vars here

    hdecl_name_map_t vname_map;
    bool add_info;
    int blkix;
    int stateix;
    hdecl_name_map_t &mod_vname_map_;

    HDLBody *xtbodyp;
    
    std::unordered_map<int, bool> Visited; // Blocks visisted to gen Method

    SCBBlock * resetblock;
    std::vector<SCBBlock *> scbblocks;
    std::vector<SCBBlock *> waitblocks;

    // pre-pass over BB to mark subexpressions
    void FindStatements(const CFGBlock &B, std::vector<const Stmt *> &SS);
    void MarkStatements(const Stmt *S, llvm::SmallDenseMap<const Stmt*, bool> &Map);
    void CheckVardecls(hNodep &hp);
    void AddThreadMethod(const CFGBlock &BI);
    void ProcessBB(const CFGBlock &BI);
   
    bool is_wait_stmt(hNodep hp);

    bool is_vardecl(hNodep hp);

    
    util lutil;

    const clang::ASTContext& ast_context_;

  };

  class SCBBlock {

  public:
    const CFGBlock& cfgb;
    std::vector<const Stmt *> stmts;
    int blkix;
    int startstmtix, endstmtix;
    std::vector<int> pred, succ;
    int statenum;
    hNodep hblockbody;
    
    
  SCBBlock(const CFGBlock& cfgbin, std::vector<const Stmt *> &SS) : cfgb{cfgbin}, stmts{SS}
    {
      blkix = statenum = -1; hblockbody = NULL;
    }
    SCBBlock(const CFGBlock& cfgbin, std::vector<const Stmt *> &SS,
	     int blki, int six, int eix, int p, int s, int stn, hNodep hb) :
    cfgb{cfgbin},
      stmts{SS},
	blkix{blki},
	  startstmtix{six}, endstmtix{eix}, statenum{stn}, hblockbody{hb}
    {
      pred.push_back(p);
      succ.push_back(s);
    }
    
    void dump();
  };

}

#endif
