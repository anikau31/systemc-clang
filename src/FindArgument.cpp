#include "FindArgument.h"

using namespace scpar;

FindArgument::FindArgument (Expr * e):_e (e) {
	TraverseStmt (e);

}

FindArgument::~FindArgument() {
  _e = nullptr;
}

bool FindArgument::VisitMemberExpr (MemberExpr * me)
{

	_argumentName = me->getMemberNameInfo ().getAsString ();
	return true;
}

bool FindArgument::VisitDeclRefExpr (DeclRefExpr * de)
{

	_argumentName = de->getFoundDecl ()->getNameAsString ();
	return true;
}

bool FindArgument::VisitIntegerLiteral (IntegerLiteral * il)
{

	_argumentName = il->getValue ().toString (10, false);
	return true;
}

bool FindArgument::VisitCXXBoolLiteralExpr (CXXBoolLiteralExpr * bl)
{
	if (bl->getValue () == false)
		{
			_argumentName = "false";
		}
	else
		{
			_argumentName = "true";
		}
	return true;
}

string FindArgument::getArgumentName ()
{
	return _argumentName;
}
