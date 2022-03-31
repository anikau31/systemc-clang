#ifndef _FIND_WAIT_H_
#define _FIND_WAIT_H_

#include "WaitContainer.h"
//#include "clang/AST/DeclCXX.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "llvm/Support/raw_ostream.h"
#include <map>

namespace clang {
  class CXXMethodDecl;
  class CallExpr;
};

namespace systemc_clang {


class FindWait : public RecursiveASTVisitor<FindWait> {
 public:
  typedef vector<WaitContainer *> waitListType;

  typedef std::pair<clang::CXXMethodDecl *, vector<std::string>>
      processWaitEventPairType;
  typedef std::map<clang::CXXMethodDecl *, vector<std::string>>
      processWaitEventMapType;
  bool VisitUnresolvedMemberExpr(clang::UnresolvedMemberExpr *e);
  FindWait(clang::CXXMethodDecl *, llvm::raw_ostream &);

  virtual ~FindWait();
  bool shouldVisitTemplateInstantiations() const;

  virtual bool VisitCallExpr(clang::CallExpr *expr);

  clang::CXXMethodDecl *getEntryMethod() const;
  waitListType getWaitCalls();
  void dump();

 private:
  clang::CXXMethodDecl *entry_method_decl_;
  llvm::raw_ostream &os_;
  clang::CallExpr *wait_call_;
  bool found_wait_;
  waitListType wait_calls_list_;
};
}  // namespace systemc_clang
#endif
