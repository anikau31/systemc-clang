#include "FindVariable.h"

using namespace scpar;

FindVariable::FindVariable(Expr *e ) 
:_e(e),
_argumentName("NONE"),
_accessType(false)
{
	TraverseStmt(e);

}

bool FindVariable::VisitMemberExpr(MemberExpr *me) {
	
	_argumentName = me->getMemberNameInfo().getAsString();
	if(me->getMemberDecl()->isDefinedOutsideFunctionOrMethod()) {
		_accessType = false;
	}
	else {
		_accessType = true;
	}
	return true;
}


bool FindVariable::VisitDeclRefExpr(DeclRefExpr *de) {
	
	_argumentName = de->getFoundDecl()->getNameAsString();
	if(de->getDecl()->isDefinedOutsideFunctionOrMethod()) {
		_accessType = false;
	}
	else {
		_accessType = true;
	}
	return true;
}


string FindVariable::getArgumentName() {
	return _argumentName;
}

bool FindVariable::getAccessType() {
	return _accessType;
}
