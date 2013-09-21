#include "FindInitiatorTarget.h"
#include "clang/AST/DeclTemplate.h"
#include "clang/AST/Type.h"
#include "clang/Basic/SourceManager.h"
#include "clang/AST/PrettyPrinter.h"

using namespace scpar;
using namespace std;

FindInitiatorTarget::FindInitiatorTarget(CXXMethodDecl *d, llvm::raw_ostream &os):
_entryMethodDecl(d)
, _os(os)
{
  TraverseDecl(d);
}

FindInitiatorTarget::~FindInitiatorTarget() {

}

bool FindInitiatorTarget::VisitStmt(Stmt *s) {
	
//	_os <<"\n Initiator dump\n";
//	s->dump();
	return true;
}

bool FindInitiatorTarget::VisitMemberExpr(MemberExpr *ce) {

	_os <<"\n New dump";
	ce->dump();
	return true;
}
