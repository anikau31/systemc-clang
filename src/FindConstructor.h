#ifndef _FIND_CONSTRUCTOR_H_
#define _FIND_CONSTRUCTOR_H_

#include <vector>
#include <map>
#include <string>

#include "clang/AST/DeclCXX.h"
#include "llvm/Support/raw_ostream.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "EntryFunctionContainer.h"

namespace scpar {

  using namespace clang;
  using namespace std;

  class FindConstructor:public RecursiveASTVisitor < FindConstructor > {
  public:

    FindConstructor( CXXRecordDecl *, llvm::raw_ostream & );
    virtual ~FindConstructor();
    virtual bool VisitCXXMethodDecl( CXXMethodDecl * );

    Stmt * returnConstructorStmt() const;
    void dump() const;

  private:
    llvm::raw_ostream &os_;
    CXXRecordDecl *declaration_;
    Stmt *constructor_stmt_;
    unsigned int pass_;

    // Private constructor
    FindConstructor( llvm::raw_ostream & );

  };
}
#endif
