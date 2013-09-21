#include "FindLoopTime.h"
#include "clang/AST/DeclTemplate.h"
#include "clang/AST/Type.h"
#include "clang/Basic/SourceManager.h"
#include "clang/AST/PrettyPrinter.h"

using namespace scpar;
using namespace std;

FindLoopTime::FindLoopTime(CXXMethodDecl *d, llvm::raw_ostream &os):
_entryMethodDecl(d)
, _os(os)
, _cpuTime(0)
, _gpuTime(0)

{
  TraverseDecl(d);
}

FindLoopTime::~FindLoopTime() {
	_forStmtLoopTimeMap.clear();
}

bool FindLoopTime::VisitForStmt(ForStmt *fs) {

	_fs = fs;
	if((_fs != NULL) &&(_cpuTime != 0) && (_gpuTime != 0)){ 
		_forStmtLoopTimeMap.insert(forStmtLoopTimePairType(_fs, 
		loopTimePairType(_cpuTime, _gpuTime)));
		_cpuTime = 0;
		_gpuTime = 0;
	}	
	return true;		
}

bool FindLoopTime::VisitCXXConstructExpr(CXXConstructExpr *expr) {

	if(expr->getConstructor()->getNameAsString() == "forLoop") {
			_os <<"\n In here";
			FloatingLiteral *cTime = 
			dyn_cast<FloatingLiteral>(expr->getArg(0)->IgnoreImpCasts());
			_cpuTime = cTime->getValue().convertToDouble();

			FloatingLiteral *gTime = 
			dyn_cast<FloatingLiteral>(expr->getArg(1)->IgnoreImpCasts());
			_gpuTime = gTime->getValue().convertToDouble();
		
		}	
	return true;
}

FindLoopTime::forStmtLoopTimeMapType FindLoopTime::returnLoopTimeMap() {
	return _forStmtLoopTimeMap;
}

void FindLoopTime::dump() {
	for(forStmtLoopTimeMapType::iterator it = _forStmtLoopTimeMap.begin(), eit = 
	_forStmtLoopTimeMap.end();
	it != eit;
	it++) {
		_os <<"\n FOR STMT : " <<it->first;
		_os <<"\n CPU TIME : " <<it->second.first << " GPU TIME : " 
		<<it->second.second;
	}
}
