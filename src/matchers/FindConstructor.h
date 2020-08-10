#ifndef _FIND_CONSTRUCTOR_H_
#define _FIND_CONSTRUCTOR_H_

#include "EntryFunctionContainer.h"
#include "clang/AST/RecursiveASTVisitor.h"

namespace systemc_clang {

using namespace clang;

class FindConstructor : public RecursiveASTVisitor<FindConstructor> {
 public:
  FindConstructor(CXXRecordDecl *, llvm::raw_ostream &);
  virtual ~FindConstructor();
  virtual bool VisitCXXMethodDecl(CXXMethodDecl *);
  virtual bool VisitCXXConstructorDecl(CXXConstructorDecl *ctor_decl);
  bool shouldVisitTemplateInstantiations() const;

  CXXRecordDecl *getAsCXXRecordDecl() const;
  CXXConstructorDecl *getConstructorDecl() const;
  Stmt *getConstructorStmt() const;
  void dump() const;

 private:
  llvm::raw_ostream &os_;
  CXXRecordDecl *declaration_;
  CXXConstructorDecl *constructor_decl_;
  Stmt *constructor_stmt_;
  unsigned int pass_;

  // Private constructor
  FindConstructor(llvm::raw_ostream &);
};
}  // namespace systemc_clang
#endif
