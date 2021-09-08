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
    hdecl_name_map_t &mod_vname_map_;

    HDLBody *xtbodyp;
    
    std::unordered_map<int, bool> Visited; // Blocks visisted to gen Method


    // pre-pass over BB to mark subexpressions
    void FindStatements(const CFGBlock &B, std::vector<const Stmt *> &SS);
    void MarkStatements(const Stmt *S, llvm::SmallDenseMap<const Stmt*, bool> &Map);
    void CheckVardecls(hNodep &hp);
    void AddThreadMethod(const CFGBlock &BI);
    void ProcessBB(const CFGBlock &BI);
    void ProcessSplitGraphBlock(const SplitCFGBlock *sgb);
    
    bool is_wait_stmt(hNodep hp);

    bool is_vardecl(hNodep hp);

    
    util lutil;

    const clang::ASTContext& ast_context_;

  };

}

#endif
