#ifndef _FIND_CONSTRUCTOR_H_
#define _FIND_CONSTRUCTOR_H_

#include <vector>
#include <string>
#include "clang/AST/DeclCXX.h"
#include "llvm/Support/raw_ostream.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "EntryFunctionContainer.h"
#include <map>
namespace scpar {

  using namespace clang;
  using namespace std;

  class FindConstructor:public RecursiveASTVisitor < FindConstructor > {
  public:

    FindConstructor(CXXRecordDecl * d, llvm::raw_ostream & os);
    virtual ~FindConstructor();

    virtual bool VisitCXXMethodDecl(CXXMethodDecl * d);

    Stmt *returnConstructorStmt();
    void dump();

  private:
    llvm::raw_ostream & _os;
    CXXRecordDecl * _d;
    Stmt *_constructorStmt;
    int pass;

    FindConstructor(llvm::raw_ostream & os);

  };
}
#endif
