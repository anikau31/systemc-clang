#ifndef _DUMP_CFG_H_
#define _DUMP_CFG_H_

#include "clang/AST/DeclCXX.h"
#include "llvm/Support/raw_ostream.h"
#include "clang/Analysis/CFG.h"
#include "clang/Analysis/CFGStmtMap.h"
#include "clang/AST/ParentMap.h"
#include "clang/AST/PrettyPrinter.h"
namespace scpar {

  using namespace clang;
  
  class CFGFindWait: public CFGStmtVisitor<CFGFindWait> {
  public:
    CFGFindWait(CFG* , llvm::raw_ostream&);
    
    void BlockStmt_VisitExpr(Expr* e);

    // This function must exist to inherit from CFGStmtVisitor.
    CFG& getCFG();
  private:
    CFG* _cfg;
    llvm::raw_ostream& _os;
  };
  
  class DumpCFG {
  public:
    DumpCFG(CXXMethodDecl*, ASTContext*, llvm::raw_ostream& );

    void dump();
    void dumpSource();
    void test();
    void splitBlock();
  private:
    CXXMethodDecl* _d;
    llvm::raw_ostream& _os;
    CFG* _cfg;
    CFGStmtMap* _stmtMap;
    
  };
}

#endif
