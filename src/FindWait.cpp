#include "FindWait.h"
#include "clang/AST/DeclTemplate.h"
#include "clang/AST/Type.h"
#include "clang/Basic/SourceManager.h"
#include "clang/AST/PrettyPrinter.h"

using namespace scpar;
using namespace std;

FindWait::FindWait(CXXMethodDecl *d, llvm::raw_ostream &os):
_entryMethodDecl(d)
, _os(os)
, _waitCall(NULL)
,_firstArg(NULL)
, _foundWait(false) 
{
  TraverseDecl(d);
}

FindWait::~FindWait() {

	_waitCalls.clear();
	_forStmtArray.clear();
	_waitTimesMap.clear();
	_processWaitEventMap.clear();	

}
bool FindWait::VisitExpr(Expr *e) {

	return true;
}
bool FindWait::VisitForStmt(ForStmt *fs) {

	unsigned int inc = 0;
	unsigned int cond = 0;
	unsigned int init = 0;
	if(_forStmtArray.size() == 0) {
		iter = 1;
		loopLevel = 1;
	}

  if(DeclStmt *ds = dyn_cast<DeclStmt>(fs->getInit())){
    Decl *d = ds->getSingleDecl();
    if(VarDecl *vd = dyn_cast<VarDecl>(d)) {
      if(IntegerLiteral *il = dyn_cast<IntegerLiteral>(vd->getInit())) {
        init =  il->getValue().getLimitedValue();
      }
    }
  }

  if(BinaryOperator *bo = dyn_cast<BinaryOperator> (fs->getCond())) {
    if(IntegerLiteral *il = dyn_cast<IntegerLiteral>(bo->getRHS())) {
      cond =  il->getValue().getLimitedValue();
    }
  }

  if(UnaryOperator *uo = dyn_cast<UnaryOperator>(fs->getInc())){
    if(uo->isIncrementOp()) {
      inc = 1;
    }
    else if (uo->isDecrementOp()) {
      inc = -1; 
		}
  }

	iter = iter*(cond-init);

  if(CompoundStmt *cs = dyn_cast<CompoundStmt>(fs->getBody())) {

   for (CompoundStmt::const_body_iterator it = cs->body_begin(), eit = cs->body_end();
    it != eit;
    it++) {
      inc++;
      Stmt *s = *it;


      if(CallExpr *ce = dyn_cast<CallExpr>(s)) {
        if(ce->getDirectCallee()->getNameInfo().getAsString() == string("wait")){
            _waitCalls.push_back(ce);
          	_waitTimesMap.insert(waitTimesPairType(ce, iter));
						_waitNestLoopMap.insert(waitNestLoopPairType(ce, loopLevel));
						_forWaitCallMap.insert(forWaitCallPairType(fs, ce));
						_foundWait = true;
        }
      }
			/* need to check surrounding data members as well around the wait guy */
			if(BinaryOperator *bo = dyn_cast<BinaryOperator>(s)) {
				if(MemberExpr *me = dyn_cast<MemberExpr>(bo->getLHS())) {
					if(_dataNestLoopMap.find(me->getMemberDecl()->getNameAsString()) == 
					_dataNestLoopMap.end()) {
						_dataNestLoopMap.insert(dataNestLoopPairType(me->getMemberDecl()->getNameAsString(), 
						loopLevel));
						_dataTimesMap.insert(dataTimesPairType(me->getMemberDecl()->getNameAsString(), 
						iter));
					}
				}
				if(ArraySubscriptExpr *as = dyn_cast<ArraySubscriptExpr>(bo->getLHS())) {
					if(MemberExpr *me = dyn_cast<MemberExpr>(as->getLHS()->IgnoreImpCasts())) {
						if(_dataNestLoopMap.find(me->getMemberDecl()->getNameAsString()) == 
						_dataNestLoopMap.end()) {
//							_dataNestLoopMap.insert(dataNestLoopPairType(me->getMemberDecl()->getNameAsString, loopLevel));
							_dataTimesMap.insert(dataTimesPairType(me->getMemberDecl()->getNameAsString(), 
							iter));
						}
					}
				}
			}
			if(ForStmt *nextFS = dyn_cast<ForStmt>(s)) {
				_forStmtArray.push_back(nextFS);	
			}
    }

  loopLevel = loopLevel + 1;
	}
	return true;		
}

bool FindWait::VisitCallExpr(CallExpr* e) {
	

  bool duplicateWait = false;
  clang::LangOptions LangOpts;
  LangOpts.CPlusPlus = true;
  clang::PrintingPolicy Policy(LangOpts);
		

  if (e->getDirectCallee()->getNameInfo().getAsString() == string("wait")) {

		for (unsigned int i = 0; i<_waitCalls.size(); i++) {
			if(e == _waitCalls.at(i)) {
				duplicateWait = true;
				break;
			}	
	}
	if(duplicateWait == false) {
	    _waitCalls.push_back(e);
	}


    if (e->getNumArgs() >= 1)  {
    string CXXMemberName = getArgumentName(e->getArg(0));
 //       _os << "## MEMBER NAME: " << CXXMemberName << "\n";
    }

    if (e->getNumArgs() == 2) {
//      _os << "@@: yeah need to find the argument\n";
			string eventName  = getArgumentName(e->getArg(0));

			if(eventName != "SC_ZERO_TIME") {
				 if(_processWaitEventMap.find(_entryMethodDecl) != _processWaitEventMap.end()) {
				 	processWaitEventMapType::iterator processFound = _processWaitEventMap.find(_entryMethodDecl);
					vector<string> tmp = processFound->second;
					tmp.push_back(eventName);
					_processWaitEventMap.erase(_entryMethodDecl);
					_processWaitEventMap.insert(processWaitEventPairType(_entryMethodDecl, tmp));
         }
				 else {
				 	vector<string> tmp;
					tmp.push_back(eventName);
					_processWaitEventMap.insert(processWaitEventPairType(_entryMethodDecl, tmp));
         }
			}
      //e->arg->dumpAll();
    } 
  }
  return true;
}

string FindWait::getArgumentName(Expr* arg) {
  if (arg  == NULL)
    return string("NULL");

  clang::LangOptions LangOpts;
  LangOpts.CPlusPlus = true;
  clang::PrintingPolicy Policy(LangOpts);
  
  string TypeS;
  llvm::raw_string_ostream s(TypeS);

  arg->printPretty(s, 0, Policy);
  //  _os << ", argument: " << s.str() << "\n";
  return s.str();
}

FindWait::waitListType FindWait::getWaitCalls() {
  return _waitCalls;
}

FindWait::waitTimesMapType FindWait::getWaitTimesMap() {
	return _waitTimesMap;
}

FindWait::dataTimesMapType FindWait::getDataTimesMap() {
	return _dataTimesMap;
}
CXXMethodDecl* FindWait::getEntryMethod() {
  return _entryMethodDecl;
}

FindWait::processWaitEventMapType FindWait::getProcessWaitEventMap() {
	return _processWaitEventMap;
}

FindWait::dataNestLoopMapType FindWait::getDataNestLoopMap() {
	return _dataNestLoopMap;
}

FindWait::waitNestLoopMapType FindWait::getWaitNestLoopMap() {
	return _waitNestLoopMap;
}


void FindWait::dump() {
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
	/*
_os <<"\n Number of times wait";
  for (waitTimesMapType::iterator it = _waitTimesMap.begin(), eit = _waitTimesMap.end();
	it != eit;
	it++) {
		_os <<it->first<<" "<<it->second<<"\n";
	}
	*/
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
/*
	_os <<"\n Wait Nest Level ";
	for (waitNestLoopMapType::iterator it = _waitNestLoopMap.begin(), eit = 
	_waitNestLoopMap.end(); it != eit; it++) {
		_os <<"\n For wait : " <<it->first<<" nest loop level is : " <<it->second;
	}

	_os <<"\n Data Nest level ";
	for (dataNestLoopMapType::iterator it = _dataNestLoopMap.begin(), eit = 
	_dataNestLoopMap.end(); it != eit; it++) {
		_os <<"\n For Data : " <<it->first<<" nest loop level is : " 
		<<it->second;

	}

*/
}
