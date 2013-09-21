#include "FindPortTracing.h"
using namespace scpar;

FindPortTracing::FindPortTracing(CXXMethodDecl* d, llvm::raw_ostream &os):
_os(os)
//, _reflectionContainer(container) 
{
  TraverseDecl(d);
}

FindPortTracing::~FindPortTracing() {
	_portTracingMap.clear();
}

bool FindPortTracing::VisitBinaryOperator (BinaryOperator *bo) {

	if(CXXMemberCallExpr *ce = 
	dyn_cast<CXXMemberCallExpr>(bo->getRHS()->IgnoreImpCasts())) {			
		if(ce->getMethodDecl()->getNameAsString() == "write") {
			
			if(MemberExpr *me = dyn_cast<MemberExpr>(bo->getLHS()->IgnoreImpCasts())){
				if(MemberExpr *cme = 
				dyn_cast<MemberExpr>(ce->getImplicitObjectArgument()->IgnoreImpCasts())) 
				{
					if(_portTracingMap.find(cme->getMemberDecl()->getNameAsString()) == _portTracingMap.end()) {
						vector<string> tmp;
						tmp.push_back(me->getMemberDecl()->getNameAsString());
						_portTracingMap.insert(portTracingPairType(cme->getMemberDecl()->getNameAsString(), 
						tmp));

					}
					else {
						portTracingMapType::iterator portFound = 
						_portTracingMap.find(cme->getMemberDecl()->getNameAsString());
						vector<string> tmp = portFound->second;
						tmp.push_back(me->getMemberDecl()->getNameAsString());
						_portTracingMap.erase(cme->getMemberDecl()->getNameAsString());
						_portTracingMap.insert(portTracingPairType(cme->getMemberDecl()->getNameAsString(), 
						tmp));
					}

				}
			}
		}
		else if(ce->getMethodDecl()->getNameAsString() == "read") {
			if(MemberExpr *me = dyn_cast<MemberExpr>(bo->getLHS()->IgnoreImpCasts())) {
				if(MemberExpr *cme = 
				dyn_cast<MemberExpr>(ce->getImplicitObjectArgument()->IgnoreImpCasts())) 
				{
					if(_portTracingMap.find(cme->getMemberDecl()->getNameAsString()) == _portTracingMap.end()) {
						vector<string> tmp;
						tmp.push_back(me->getMemberDecl()->getNameAsString());
						_portTracingMap.insert(portTracingPairType(cme->getMemberDecl()->getNameAsString(), 
						tmp));				
					}
					else {
						portTracingMapType::iterator portFound = 
						_portTracingMap.find(cme->getMemberDecl()->getNameAsString());
						vector<string> tmp = portFound->second;
						tmp.push_back(me->getMemberDecl()->getNameAsString());
						_portTracingMap.erase(cme->getMemberDecl()->getNameAsString());
						_portTracingMap.insert(portTracingPairType(cme->getMemberDecl()->getNameAsString(), 
						tmp));
					}

				}
			}
		}		
	}
	return true;
}

FindPortTracing::portTracingMapType FindPortTracing::getPortTraceMap() {
	return _portTracingMap;
}

void FindPortTracing::dump() {

	for(portTracingMapType::iterator it = _portTracingMap.begin(), eit = 
		_portTracingMap.end();
		it != eit;
		it++) {
		_os <<"\n For port : " <<it->first;
		vector<string> tmp = it->second;
		_os <<"  Variables reading or writing to port : \n"; 
		for (int i = 0; i<tmp.size(); i++) {
			_os<<"  "<<tmp.at(i);
		}
	}		
}
