#include "FindSensitivity.h"
#include "FindTemplateTypes.h"
using namespace scpar;

FindSensitivity::FindSensitivity (Stmt * s, llvm::raw_ostream & os):
  _os (os),
	_foundSensitiveNode(false) {

	/// Pass 1
	/// Find the ASTNode CXXOperatorCallExpr
	TraverseStmt(s);
}

bool FindSensitivity::VisitCXXOperatorCallExpr (CXXOperatorCallExpr *e) {
	return true;
}

bool FindSensitivity::VisitMemberExpr (MemberExpr *e) {

	QualType q = e->getType ();

	if (!_foundSensitiveNode)
		{
			string memberName = e->getMemberDecl ()->getNameAsString ();

			if ((q.getAsString () != "class sc_core::sc_sensitive")
					|| (memberName != "sensitive"))	{
					return true;
				}
		}
	/// This is an sensitivity declaration.
	_foundSensitiveNode = true;
	//  _os << "####: VisitMemberExpr -- " << e->getMemberDecl()->getNameAsString() << "\n";
	//e->dumpAll();

	/// Check if this expression's got a type of port.
	QualType memberType = e->getMemberDecl ()->getType ();
	FindTemplateTypes tt;

	tt.Enumerate (memberType.getTypePtr ());

	FindTemplateTypes::argVectorType args = tt.getTemplateArgumentsType ();
	FindTemplateTypes::argVectorType::iterator ait = args.begin ();

	if (args.size () == 0)
		return true;

	/// Is it a port type
	if (!	(ait->first == "sc_in" || ait->first == "sc_out"
			 || ait->first == "sc_inout"))	{
			return true;
		}

	/// It is a port type. Now we can do whatever we need to do to get the port name.
	//  _os << "@@@@@: " << memberType.getAsString() << ", " << ait->first << "\n";

	_sensitivePorts.insert (kvType (e->getMemberDecl ()->getNameAsString (), e));
//  _reflectionContainer->setChildEntries(new ReflectionContainerClass(e->getMemberDecl()->getNameAsString(), e));
	return true;
}

void  FindSensitivity::dump() {
	_os << "\n ==================== Find Sensitivity ===================";
	_os << "\n:> Print Sensitive Ports";
	for (senseMapType::iterator mit = _sensitivePorts.begin (), mitend =
			 _sensitivePorts.end (); mit != mitend; mit++)
		{
			_os << "\n:> name: " << mit->first << ", MemberExpr*: " << mit->second;
		}
	_os << "\n ==================== END Find Sensitivity ===================";
}

FindSensitivity::senseMapType FindSensitivity::getSenseMap() {
 return _sensitivePorts;
}

FindSensitivity::~FindSensitivity() {

}
