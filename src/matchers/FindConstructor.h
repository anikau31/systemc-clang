#ifndef _FIND_CONSTRUCTOR_H_
#define _FIND_CONSTRUCTOR_H_

#include "EntryFunctionContainer.h"
#include "clang/AST/RecursiveASTVisitor.h"

namespace systemc_clang {

class FindConstructor : public clang::RecursiveASTVisitor<FindConstructor> {
 public:
  FindConstructor(const clang::CXXRecordDecl *, llvm::raw_ostream &);
  virtual ~FindConstructor();
  virtual bool VisitCXXMethodDecl(clang::CXXMethodDecl *);
  virtual bool VisitCXXConstructorDecl(clang::CXXConstructorDecl *ctor_decl);
  bool shouldVisitTemplateInstantiations() const;

  const clang::CXXRecordDecl *getAsCXXRecordDecl() const;
  clang::CXXConstructorDecl *getConstructorDecl() const;
  clang::Stmt *getConstructorStmt() const;
  void dump() const;

 private:
  llvm::raw_ostream &os_;
  const clang::CXXRecordDecl *declaration_;
  clang::CXXConstructorDecl *constructor_decl_;
  clang::Stmt *constructor_stmt_;
  unsigned int pass_;

  // Private constructor
  FindConstructor(llvm::raw_ostream &);
};
}  // namespace systemc_clang
#endif
