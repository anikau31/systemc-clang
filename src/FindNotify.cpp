#include "FindNotify.h"
#include "clang/AST/DeclTemplate.h"
#include "clang/AST/Type.h"
#include "clang/Basic/SourceManager.h"
#include "clang/AST/PrettyPrinter.h"

using namespace scpar;
using namespace std;

FindNotify::FindNotify( CXXMethodDecl *d, llvm::raw_ostream &os ) :
  entry_method_decl_(d),
  os_(os),
  notify_call_{nullptr} {
	TraverseDecl(d);
}

FindNotify::~FindNotify() {
	notify_call_list_.clear();
}

bool FindNotify::VisitCallExpr( CallExpr * e ) {
	//  e->dumpAll();
	LangOptions	LangOpts;

	LangOpts.CPlusPlus = true;
	PrintingPolicy	Policy(LangOpts);

	if (e->getDirectCallee()->getNameInfo().getAsString() == string ("notify") && e->getNumArgs () <= 2)		{														// need a better checking.....
    notify_call_list_.push_back(e);
    // To get the 'x' from x.f(5) I must use getImplicitObjectArgument. 

    /*
      string exprName; 
      if(MemberExpr *me = dyn_cast<MemberExpr>(e->getCallee())) {
      exprName = getArgumentName(me->getBase()->IgnoreImpCasts());
      if(_processNotifyEventMap.find(entry_method_decl_) != _processNotifyEventMap.end()){

      processNotifyEventMapType::iterator processFound = _processNotifyEventMap.find(entry_method_decl_);
      vector<string> tmp = processFound->second; 
      tmp.push_back(exprName);
      _processNotifyEventMap.erase(entry_method_decl_);
      _processNotifyEventMap.insert(processNotifyEventPairType(entry_method_decl_, tmp));
      }
      else {
      vector<string> tmp;
      tmp.push_back(exprName);
      _processNotifyEventMap.insert(processNotifyEventPairType(entry_method_decl_, tmp));
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

CXXMethodDecl * FindNotify::getEntryMethod() const {
  return entry_method_decl_;
}

FindNotify::NotifyCallListType FindNotify::getNotifyCallList() const {
	return notify_call_list_;
}

void FindNotify::dump() const {
  /*  
      os_ << "\n ============== FindNotify ===============";
      os_ << "\n:> Print 'notify' statement informtion\n";
      for (unsigned int i = 0; i < notify_call_list_.size(); i++) {
      os_ << ":> notify pointer: " <<  notify_call_list_[i] << ", implicit arg: " << \
      getArgumentName(notify_call_list_[i]->getCallee()) << "\n";

      }
      os_ << "\n ============== END FindNotify ===============";
      os_ <<"\n Process and events they notify";
	
      for(processNotifyEventMapType::iterator it = _processNotifyEventMap.begin(), eit = _processNotifyEventMap.end();
			it != eit;
			it++) {
			os_ <<"\n Process : " <<it->first->getDeclName().getAsString();
			os_ <<"\n Event Notification: ";
			vector<string> tmp = it->second;
			for (int i =0; i<tmp.size(); i++) {
      os_ <<tmp.at(i)<<" ";
			}
      }
  */
}
