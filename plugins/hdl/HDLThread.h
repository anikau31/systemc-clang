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

    hdecl_name_map_t vname_map;
    bool add_info;
    std::vector<SCBBlock *> scblocks;
    
    // pre-pass over BB to mark subexpressions
    void findStatements(CFGBlock *B, std::vector<const Stmt *> &SS);
    void markStatements(const Stmt *S, llvm::SmallDenseMap<const Stmt*, bool> &Map);
    bool is_wait_stmt(hNodep hp);
    
    util lutil;

    const ASTContext& ast_context_;
  };

  class SCBBlock {

  public:
    CFGBlock* cfgb;
    std::vector<const Stmt *> stmts;
    int blkix;
    int startstmtix, endstmtix;
    std::vector<int> pred, succ;
    int statenum;
    int wait_reset; // -1=>reset, 0=>normal, >=1 means this entry is a wait statement
    
    
  SCBBlock(CFGBlock* cfgbin, std::vector<const Stmt *> &SS) : cfgb{cfgbin}, stmts{SS} {}
    void dump();
    void set_reset() { wait_reset = -1;}
    bool is_reset() { return (wait_reset == -1);}
    void set_normal() { wait_reset = 0;}
    bool is_normal() { return (wait_reset == 0);}
    void set_wr(int clocks=1) { wait_reset = clocks; }
    int get_wr() { return wait_reset;}
      
  };

}

#endif
