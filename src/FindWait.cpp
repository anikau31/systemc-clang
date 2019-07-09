#include "FindWait.h"
#include "clang/AST/DeclTemplate.h"
#include "clang/AST/PrettyPrinter.h"
#include "clang/AST/Type.h"
#include "clang/Basic/SourceManager.h"

using namespace scpar;
using namespace std;

FindWait::FindWait(CXXMethodDecl *d, llvm::raw_ostream &os)
    : entry_method_decl_{d}, os_{os}, wait_call_{nullptr},
      found_wait_{false} {
          TraverseDecl(d);

          // If there was a wait that was found.  Then go through and dump them all out?
          if (found_wait_) {
              for (auto wait  : wait_calls_list_) {
                  wait->dump(os_,2);
              }
          }
      }

FindWait::~FindWait() {
    // Delete all the pointers.
    for (auto wait : wait_calls_list_ ) {
        delete wait;
    }
    wait_calls_list_.clear();
}


bool FindWait::VisitUnresolvedMemberExpr(UnresolvedMemberExpr *e) {

    // This is for the templated wait calls.
    // e should not be null if it gets here.
    if (e->getMemberNameInfo().getAsString() == string("wait") ) {

        wait_calls_list_.push_back(new WaitContainer(entry_method_decl_, wait_call_));
        found_wait_ = true;
    }

    return true;
}

bool FindWait::VisitCallExpr(CallExpr *e) {

    clang::LangOptions LangOpts;
    LangOpts.CPlusPlus = true;
    clang::PrintingPolicy Policy(LangOpts);

    // This allows templated wait calls to refer to the CallExpr as well.
    wait_call_ = e;
    // Check if the non-templated wait has a "wait" in it.
    auto direct_callee{ e->getDirectCallee() };
    if ( (direct_callee != nullptr) ) {
        if ( (direct_callee->getNameInfo().getAsString() == string("wait") ) ) {

            // Insert the information to parse the wait arguments.
            wait_calls_list_.push_back(new WaitContainer(entry_method_decl_, wait_call_));
            found_wait_ = true;
        }
    }

    return true;
}

FindWait::waitListType FindWait::getWaitCalls() { return wait_calls_list_; }

CXXMethodDecl *FindWait::getEntryMethod() const { return entry_method_decl_; }

void FindWait::dump() {
    /*
      os_ << "\n ============== FindWait ===============";
      os_ << "\n:> Print 'wait' statement informtion\n";
      for (unsigned int i = 0; i < wait_calls_list_.size(); i++) {
      if(wait_calls_list_.size() > 2) {
      os_ << ":> wait pointer: " <<  wait_calls_list_[i] << "num. of args: " <<
      wait_calls_list_[i]->getNumArgs()-1 << "\n";

      for (unsigned int j = 0; j < wait_calls_list_[i]->getNumArgs()-1; j++) {
      os_ << "- arg " << j << ": " << getArgumentName(wait_calls_list_[i]->getArg(j))
      << "\n";
      }
      }
      else {
      os_ << ":> wait pointer: " <<  wait_calls_list_[i] << "num. of args: " <<
      wait_calls_list_[i]->getNumArgs() << "\n";

      for (unsigned int j = 0; j < wait_calls_list_[i]->getNumArgs(); j++) {
      os_ << "- arg " << j << ": " << getArgumentName(wait_calls_list_[i]->getArg(j))
      << "\n";
      }
      }
      }

      os_ << "\n ============== END FindWait ===============";

      os_ <<"\n Process and waiting on events ";
      for (processWaitEventMapType::iterator it = _processWaitEventMap.begin(), eit =
      _processWaitEventMap.end(); it != eit; it++) { os_ <<"\n Process :"
      <<it->first->getDeclName().getAsString();

      os_ <<"\n Waiting on event(s) : ";
      vector<string> tmp = it->second;
      for (int i = 0; i<tmp.size(); i++) {
      os_ <<tmp.at(i)<<" ";
      }
      }
    */
}
