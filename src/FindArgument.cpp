#include "FindArgument.h"

using namespace scpar;

FindArgument::FindArgument(Expr * e) :
  expression_(e) {
  TraverseStmt(expression_);
}

FindArgument::~FindArgument() {
  expression_ = nullptr;
}

bool FindArgument::VisitMemberExpr ( MemberExpr *member_expr ) {
	argument_name_ = member_expr->getMemberNameInfo().getAsString();
	return true;
}

bool FindArgument::VisitDeclRefExpr( DeclRefExpr *declaration_ref ) {
	argument_name_ = declaration_ref->getFoundDecl()->getNameAsString();
	return true;
}

bool FindArgument::VisitIntegerLiteral( IntegerLiteral *integer_literal ) {
	argument_name_ = integer_literal->getValue().toString(10, false);
	return true;
}

bool FindArgument::VisitCXXBoolLiteralExpr( CXXBoolLiteralExpr *boolean_expr ) {
	if ( boolean_expr->getValue() == false )	{
			argument_name_ = "false";
		}
	else {
			argument_name_ = "true";
		}
	return true;
}

string FindArgument::getArgumentName() {
	return argument_name_;
}
