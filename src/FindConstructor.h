#ifndef _FIND_CONSTRUCTOR_H_
#define _FIND_CONSTRUCTOR_H_

#include "EntryFunctionContainer.h"
#include "clang/AST/RecursiveASTVisitor.h"

namespace scpar {

using namespace clang;

class FindConstructor : public RecursiveASTVisitor<FindConstructor> {
 public:
  FindConstructor(CXXRecordDecl *, llvm::raw_ostream &);
  virtual ~FindConstructor();
  virtual bool VisitCXXMethodDecl(CXXMethodDecl *);
  bool shouldVisitTemplateInstantiations() const;

  Stmt *getConstructorStmt() const;
  void dump() const;

 private:
  llvm::raw_ostream &os_;
  CXXRecordDecl *declaration_;
  Stmt *constructor_stmt_;
  unsigned int pass_;

  // Private constructor
  FindConstructor(llvm::raw_ostream &);
};
}  // namespace scpar
#endif
