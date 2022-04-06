#include "FindArgument.h"

#include "clang/AST/DeclCXX.h"
#include "llvm/Support/raw_ostream.h"
#include "APIntUtils.h"

using namespace systemc_clang;
using namespace clang;

FindArgument::FindArgument(Expr *e) : expression_(e) {
  TraverseStmt(expression_);
}

FindArgument::~FindArgument() { expression_ = nullptr; }

bool FindArgument::VisitMemberExpr(MemberExpr *member_expr) {
  argument_name_ = member_expr->getMemberNameInfo().getAsString();
  return true;
}

bool FindArgument::VisitDeclRefExpr(DeclRefExpr *declaration_ref) {
  argument_name_ = declaration_ref->getFoundDecl()->getNameAsString();
  return true;
}

bool FindArgument::VisitIntegerLiteral(IntegerLiteral *integer_literal) {
  argument_name_ =
      systemc_clang::utils::apint::toString(integer_literal->getValue());
  return true;
}

bool FindArgument::VisitCXXBoolLiteralExpr(CXXBoolLiteralExpr *boolean_expr) {
  if (boolean_expr->getValue() == false) {
    argument_name_ = "false";
  } else {
    argument_name_ = "true";
  }
  return true;
}

std::string FindArgument::getArgumentName() { return argument_name_; }
