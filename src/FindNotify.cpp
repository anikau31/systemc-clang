#include "FindNotify.h"
#include "clang/AST/DeclTemplate.h"
#include "clang/AST/Type.h"
#include "clang/Basic/SourceManager.h"
#include "clang/AST/PrettyPrinter.h"

using namespace scpar;
using namespace std;

FindNotify::FindNotify (CXXMethodDecl * d, llvm::raw_ostream & os):
_entryMethodDecl (d), _os (os), _notifyCall (NULL)
{
	TraverseDecl (d);
}

FindNotify::~FindNotify ()
{
	_notifyCalls.clear ();

}

bool FindNotify::VisitCallExpr (CallExpr * e)
{
	//  e->dumpAll();
	LangOptions
		LangOpts;

	LangOpts.CPlusPlus = true;
	PrintingPolicy
	Policy (LangOpts
	);

	if (e->getDirectCallee ()->getNameInfo ().getAsString () ==
			string ("notify") && e->getNumArgs () <= 2)
		{														// need a better checking.....
			_notifyCalls.push_back (e);
			// To get the 'x' from x.f(5) I must use getImplicitObjectArgument. 

			/*
			   string exprName; 
			   if(MemberExpr *me = dyn_cast<MemberExpr>(e->getCallee())) {
			   exprName = getArgumentName(me->getBase()->IgnoreImpCasts());
			   if(_processNotifyEventMap.find(_entryMethodDecl) != _processNotifyEventMap.end()){

			   processNotifyEventMapType::iterator processFound = _processNotifyEventMap.find(_entryMethodDecl);
			   vector<string> tmp = processFound->second; 
			   tmp.push_back(exprName);
			   _processNotifyEventMap.erase(_entryMethodDecl);
			   _processNotifyEventMap.insert(processNotifyEventPairType(_entryMethodDecl, tmp));
			   }
			   else {
			   vector<string> tmp;
			   tmp.push_back(exprName);
			   _processNotifyEventMap.insert(processNotifyEventPairType(_entryMethodDecl, tmp));
			   }
			   }
			 */
		}
	return true;
}

/*
string FindNotify::getArgumentName(Expr* arg) {
  if (arg  == NULL) {
    return string("NULL");
  }

  LangOptions LangOpts;
  LangOpts.CPlusPlus = true;
  PrintingPolicy Policy(LangOpts);
  
  string TypeS;
  llvm::raw_string_ostream s(TypeS);

  arg->printPretty(s, 0, Policy);
  return s.str();
}
*/

CXXMethodDecl * FindNotify::getEntryMethod() {
 return _entryMethodDecl; 
}

FindNotify::notifyCallListType FindNotify::getNotifyCallList ()
{
	return _notifyCalls;
}

void
  FindNotify::dump (
)
{
/*  
  _os << "\n ============== FindNotify ===============";
  _os << "\n:> Print 'notify' statement informtion\n";
  for (unsigned int i = 0; i < _notifyCalls.size(); i++) {
    _os << ":> notify pointer: " <<  _notifyCalls[i] << ", implicit arg: " << \
      getArgumentName(_notifyCalls[i]->getCallee()) << "\n";

  }
  _os << "\n ============== END FindNotify ===============";
	_os <<"\n Process and events they notify";
	
	for(processNotifyEventMapType::iterator it = _processNotifyEventMap.begin(), eit = _processNotifyEventMap.end();
			it != eit;
			it++) {
			_os <<"\n Process : " <<it->first->getDeclName().getAsString();
			_os <<"\n Event Notification: ";
			vector<string> tmp = it->second;
			for (int i =0; i<tmp.size(); i++) {
				_os <<tmp.at(i)<<" ";
			}
	}
*/
}
