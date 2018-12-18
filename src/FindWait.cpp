#include "FindWait.h"
#include "clang/AST/DeclTemplate.h"
#include "clang/AST/Type.h"
#include "clang/Basic/SourceManager.h"
#include "clang/AST/PrettyPrinter.h"

using namespace scpar;
using namespace std;

FindWait::FindWait( CXXMethodDecl *d, llvm::raw_ostream &os ) :
  entry_method_decl_{d},
  os_{os},
  wait_call_{nullptr} { //},
  //  first_arg_{nullptr},
  //  found_wait_{false} {
    TraverseDecl(d);
}

FindWait::~FindWait() {
	wait_calls_list_.clear();
}

bool FindWait::VisitCallExpr ( CallExpr *e ) {

  //	bool duplicateWait = false;

	clang::LangOptions LangOpts;
	LangOpts.CPlusPlus = true;
	clang::PrintingPolicy Policy (LangOpts);

	if (e->getDirectCallee()->getNameInfo().getAsString() == string("wait"))	{
    wait_calls_list_.push_back (e);
  }
	return true;
}

FindWait::waitListType FindWait::getWaitCalls() {
	return wait_calls_list_;
}

CXXMethodDecl* FindWait::getEntryMethod() const {
	return entry_method_decl_;
}

void FindWait::dump() {
	/*
    os_ << "\n ============== FindWait ===============";
    os_ << "\n:> Print 'wait' statement informtion\n";
    for (unsigned int i = 0; i < wait_calls_list_.size(); i++) {
    if(wait_calls_list_.size() > 2) {
    os_ << ":> wait pointer: " <<  wait_calls_list_[i] << "num. of args: " << wait_calls_list_[i]->getNumArgs()-1 << "\n";

    for (unsigned int j = 0; j < wait_calls_list_[i]->getNumArgs()-1; j++) {
    os_ << "- arg " << j << ": " << getArgumentName(wait_calls_list_[i]->getArg(j)) << "\n";
    }
    }
    else {
    os_ << ":> wait pointer: " <<  wait_calls_list_[i] << "num. of args: " << wait_calls_list_[i]->getNumArgs() << "\n";

    for (unsigned int j = 0; j < wait_calls_list_[i]->getNumArgs(); j++) {
    os_ << "- arg " << j << ": " << getArgumentName(wait_calls_list_[i]->getArg(j)) << "\n";
    }  
    }
    }

    os_ << "\n ============== END FindWait ===============";

    os_ <<"\n Process and waiting on events ";
    for (processWaitEventMapType::iterator it = _processWaitEventMap.begin(), eit = _processWaitEventMap.end();
    it != eit;
    it++) {
    os_ <<"\n Process :" <<it->first->getDeclName().getAsString();

    os_ <<"\n Waiting on event(s) : ";
    vector<string> tmp = it->second;
    for (int i = 0; i<tmp.size(); i++) {
    os_ <<tmp.at(i)<<" ";
    }
    }
  */
}
