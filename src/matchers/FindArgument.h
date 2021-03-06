#ifndef _FIND_ARGUMENT_
#define _FIND_ARGUMENT_

#include "clang/AST/DeclCXX.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "llvm/Support/raw_ostream.h"

namespace systemc_clang {
using namespace clang;
using namespace std;

class FindArgument : public RecursiveASTVisitor<FindArgument> {

public:
  FindArgument(Expr *);
  virtual ~FindArgument();
  virtual bool VisitMemberExpr(MemberExpr *me);
  virtual bool VisitDeclRefExpr(DeclRefExpr *de);
  virtual bool VisitIntegerLiteral(IntegerLiteral *il);
  virtual bool VisitCXXBoolLiteralExpr(CXXBoolLiteralExpr *bl);



  string getArgumentName();

private:
  Expr *expression_;
  string argument_name_;
};
} // namespace systemc_clang
#endif
