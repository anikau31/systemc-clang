#ifndef _FIND_WAIT_H_
#define _FIND_WAIT_H_

#include "clang/AST/DeclCXX.h"
#include "llvm/Support/raw_ostream.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include <map>
namespace scpar {

  using namespace clang;
  using namespace std;

  class FindWait:public RecursiveASTVisitor < FindWait > {
  public:

    typedef vector < CallExpr * >waitListType;

    typedef pair < CXXMethodDecl *,vector < string > >processWaitEventPairType;
    typedef map < CXXMethodDecl *, vector < string > >processWaitEventMapType;

    FindWait( CXXMethodDecl *, llvm::raw_ostream & );
    virtual ~FindWait();

    virtual bool VisitCallExpr( CallExpr *expr );

    CXXMethodDecl* getEntryMethod() const;
    waitListType getWaitCalls();
    void dump();

  private:
    CXXMethodDecl *entry_method_decl_;
    llvm::raw_ostream & os_;
    CallExpr *wait_call_;
    //    Expr *first_arg_;
    //    bool found_wait_;
    waitListType wait_calls_list_;

  };
}
#endif
