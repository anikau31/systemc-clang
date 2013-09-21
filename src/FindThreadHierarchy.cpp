#include "FindThreadHierarchy.h"
#include "clang/AST/DeclTemplate.h"
#include "clang/AST/Type.h"
#include "clang/Basic/SourceManager.h"
#include "clang/AST/PrettyPrinter.h"

using namespace scpar;
using namespace std;

FindThreadHierarchy::FindThreadHierarchy(CXXMethodDecl *d, llvm::raw_ostream &os):
_entryMethodDecl(d)
, _os(os)

{

  TraverseDecl(d);
}

FindThreadHierarchy::~FindThreadHierarchy() {
	_forStmtThreads.clear();
	_blockDimensions.clear();
	_gridDimensions.clear();
}

bool FindThreadHierarchy::VisitForStmt(ForStmt *fs) {

	_fs = fs;
	if((_fs != NULL)&&(_blockDimensions.size() != 0)&&(_gridDimensions.size() != 0)) { 
		_forStmtThreads.insert(forStmtThreadHierarchyPairType(_fs, threadHierarchy(_blockDimensions, _gridDimensions)));
		_blockDimensions.clear();
		_gridDimensions.clear();
	}	
	return true;		
}

bool FindThreadHierarchy::VisitCXXConstructExpr(CXXConstructExpr *expr) {

	if(expr->getConstructor()->getNameAsString() == "sc_gpu_thread_hierarchy") {
		for (unsigned int i = 0; i<expr->getNumArgs(); i++) {
			IntegerLiteral *il = 
			dyn_cast<IntegerLiteral>(expr->getArg(i)->IgnoreImpCasts());
			_blockDimensions.push_back(il->getValue().getLimitedValue());				
		}
	}
	if(expr->getConstructor()->getNameAsString() == "sc_gpu_block_hierarchy") {
		for (unsigned int i = 0 ; i<expr->getNumArgs(); i++) {
				IntegerLiteral *il = 
				dyn_cast<IntegerLiteral>(expr->getArg(i)->IgnoreImpCasts());
				_gridDimensions.push_back(il->getValue().getLimitedValue());
		}	
	}
	return true;
}

FindThreadHierarchy::forStmtThreadHierarchyMapType FindThreadHierarchy::returnThreadHierarchy() {
	return _forStmtThreads;
}

void FindThreadHierarchy::dump() {
	for (forStmtThreadHierarchyMapType::iterator it = _forStmtThreads.begin(), eit = _forStmtThreads.end();
				it != eit;
				it++) {
					
		threadHierarchy tmpThreadHierarchy = it->second;
		_os <<"\n For FORSTMT : " <<it->first;
		_os <<"\n Block Dimensions : " ;
		for (unsigned int i = 0; i< tmpThreadHierarchy.first.size(); i++) {
			_os <<tmpThreadHierarchy.first.at(i)<<" ";
		}
		_os <<"\n Grid Dimensions : " ;
		for (unsigned int i = 0; i<tmpThreadHierarchy.second.size(); i++) {				
			_os <<tmpThreadHierarchy.second.at(i)<<" ";
		}
	}
}
