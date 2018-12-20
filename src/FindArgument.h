#ifndef _FIND_ARGUMENT_
#define _FIND_ARGUMENT_

#include "clang/AST/DeclCXX.h"
#include "llvm/Support/raw_ostream.h"
#include "clang/AST/RecursiveASTVisitor.h"

namespace scpar {
  using namespace clang;
  using namespace std;

  class FindArgument:public RecursiveASTVisitor < FindArgument > {

  public:
    FindArgument( Expr * );
    virtual ~FindArgument();
    virtual bool VisitMemberExpr (MemberExpr * me);
    virtual bool VisitDeclRefExpr (DeclRefExpr * de);
    virtual bool VisitIntegerLiteral (IntegerLiteral * il);
    virtual bool VisitCXXBoolLiteralExpr (CXXBoolLiteralExpr * bl);

    string getArgumentName();
  private:
    Expr * expression_;
    string argument_name_;
  };
}
#endif
