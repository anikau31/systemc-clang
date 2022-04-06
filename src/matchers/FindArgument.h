#ifndef _FIND_ARGUMENT_
#define _FIND_ARGUMENT_

#include "clang/AST/RecursiveASTVisitor.h"

namespace clang {
  class Expr;
  class MemberExpr;
  class DeclRefExpr;
  class IntegerLiteral;
  class CXXBoolLiteralExpr;

};

namespace systemc_clang {

class FindArgument : public clang::RecursiveASTVisitor<FindArgument> {

public:
  FindArgument(clang::Expr *);
  virtual ~FindArgument();
  virtual bool VisitMemberExpr(clang::MemberExpr *me);
  virtual bool VisitDeclRefExpr(clang::DeclRefExpr *de);
  virtual bool VisitIntegerLiteral(clang::IntegerLiteral *il);
  virtual bool VisitCXXBoolLiteralExpr(clang::CXXBoolLiteralExpr *bl);



  std::string getArgumentName();

private:
  clang::Expr *expression_;
  std::string argument_name_;
};
} // namespace systemc_clang
#endif
