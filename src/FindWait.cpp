#include "FindWait.h"
#include "clang/AST/DeclTemplate.h"
#include "clang/AST/Type.h"
#include "clang/Basic/SourceManager.h"
#include "clang/AST/PrettyPrinter.h"

using namespace scpar;
using namespace std;

FindWait::FindWait (CXXMethodDecl *d, llvm::raw_ostream &os):
_entryMethodDecl(d),
_os(os),
_waitCall(nullptr),
_firstArg(nullptr),
_foundWait (false) {
	TraverseDecl(d);
}

FindWait::~FindWait() {
	_waitCalls.clear ();
}

bool FindWait::VisitCallExpr (CallExpr *e) {

  //	bool duplicateWait = false;

	clang::LangOptions LangOpts;
	LangOpts.CPlusPlus = true;
	clang::PrintingPolicy Policy (LangOpts);

	if (e->getDirectCallee ()->getNameInfo ().getAsString () == string ("wait"))	{
			_waitCalls.push_back (e);
		}
	return true;
}

FindWait::waitListType FindWait::getWaitCalls() {
	return _waitCalls;
}

CXXMethodDecl* FindWait::getEntryMethod() {
	return _entryMethodDecl;
}


void FindWait::dump() {
	/*
	   _os << "\n ============== FindWait ===============";
	   _os << "\n:> Print 'wait' statement informtion\n";
	   for (unsigned int i = 0; i < _waitCalls.size(); i++) {
	   if(_waitCalls.size() > 2) {
	   _os << ":> wait pointer: " <<  _waitCalls[i] << "num. of args: " << _waitCalls[i]->getNumArgs()-1 << "\n";

	   for (unsigned int j = 0; j < _waitCalls[i]->getNumArgs()-1; j++) {
	   _os << "- arg " << j << ": " << getArgumentName(_waitCalls[i]->getArg(j)) << "\n";
	   }
	   }
	   else {
	   _os << ":> wait pointer: " <<  _waitCalls[i] << "num. of args: " << _waitCalls[i]->getNumArgs() << "\n";

	   for (unsigned int j = 0; j < _waitCalls[i]->getNumArgs(); j++) {
	   _os << "- arg " << j << ": " << getArgumentName(_waitCalls[i]->getArg(j)) << "\n";
	   }  
	   }
	   }

	   _os << "\n ============== END FindWait ===============";

	   _os <<"\n Process and waiting on events ";
	   for (processWaitEventMapType::iterator it = _processWaitEventMap.begin(), eit = _processWaitEventMap.end();
	   it != eit;
	   it++) {
	   _os <<"\n Process :" <<it->first->getDeclName().getAsString();

	   _os <<"\n Waiting on event(s) : ";
	   vector<string> tmp = it->second;
	   for (int i = 0; i<tmp.size(); i++) {
	   _os <<tmp.at(i)<<" ";
	   }
	   }
	 */
}
