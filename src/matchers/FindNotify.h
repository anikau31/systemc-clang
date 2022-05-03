#ifndef _FIND_NOTIFY_H_
#define _FIND_NOTIFY_H_

//#include "clang/AST/DeclCXX.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "llvm/Support/raw_ostream.h"

namespace clang {
  class CXXMethodDecl;
  class CallExpr;

};



namespace systemc_clang {

class FindNotify : public clang::RecursiveASTVisitor<FindNotify> {
 public:
  using  NotifyCallListType = std::vector<clang::CallExpr *>; 

  FindNotify(clang::CXXMethodDecl *, llvm::raw_ostream &);
  virtual ~FindNotify();

  virtual bool VisitCallExpr(clang::CallExpr *expr);
  bool shouldVisitTemplateInstantiations() const;

  NotifyCallListType getNotifyCallList() const;
  clang::CXXMethodDecl *getEntryMethod() const;

  void dump() const;

 private:
  clang::CXXMethodDecl *entry_method_decl_;
  llvm::raw_ostream &os_;
  clang::CallExpr *notify_call_;
  NotifyCallListType notify_call_list_;
};
}  // namespace systemc_clang
#endif
