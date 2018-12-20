#ifndef _FIND_NOTIFY_H_
#define _FIND_NOTIFY_H_

#include "clang/AST/DeclCXX.h"
#include "llvm/Support/raw_ostream.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include <map>
namespace scpar {

  using namespace clang;
  using namespace std;

  class FindNotify:public RecursiveASTVisitor < FindNotify > {
  public:
    typedef vector < CallExpr * > NotifyCallListType;

    FindNotify( CXXMethodDecl *, raw_ostream & );
    virtual ~FindNotify();
    virtual bool VisitCallExpr ( CallExpr *expr );

    NotifyCallListType getNotifyCallList() const;
    CXXMethodDecl * getEntryMethod() const;
    void dump() const;

  private:
    CXXMethodDecl *entry_method_decl_;
    llvm::raw_ostream &os_;
    CallExpr *notify_call_;
    NotifyCallListType notify_call_list_;
  };
}
#endif
