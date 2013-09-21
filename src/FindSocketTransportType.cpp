#include "FindSocketTransportType.h"
#include "clang/AST/DeclTemplate.h"
#include "FindArgument.h"
using namespace scpar;
using namespace std;

Properties::Properties(string transportType, string payloadName, string phase, 
string delay)
:_transportType(transportType)
,_payloadName(payloadName)
, _phase(phase)
,_delay(delay)
{


}

Properties::Properties(string transportType, string payloadName, string delay)
:_transportType(transportType)
,_payloadName(payloadName)
, _delay(delay)
{

}

Properties::~Properties() {
	_transportType.clear();
	_delay.clear();
	_payloadName.clear();
}

void Properties::dump(raw_ostream& os) {
	
	os << "\n Transport Type : " <<_transportType<<" Payload : " <<_payloadName<<" Delay : " <<_delay <<" Phase : " <<_phase;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////
FindSocketTransportType::FindSocketTransportType(CXXMethodDecl *d, llvm::raw_ostream& os) 
:_d(d),
_os(os)
{
	
	TraverseDecl(_d);
}

FindSocketTransportType::~FindSocketTransportType() {
	for (socketTransportMapType::iterator it = _socketTransportMap.begin(), eit = _socketTransportMap.end();
	it!= eit;
	it++) {
		delete it->second;
	}
}

bool FindSocketTransportType::VisitCXXMemberCallExpr(CXXMemberCallExpr *ce) {
	
	string transportType;
	string delay;
	string socketName;
	string phase;
	string payloadName;

	if(ce->getMethodDecl()->getNameAsString() == "b_transport" || 
	ce->getMethodDecl()->getNameAsString() == "get_direct_mem_ptr" || 
	ce->getMethodDecl()->getNameAsString()=="invalidate_direct_mem_ptr") {
		transportType = ce->getMethodDecl()->getNameAsString();
		if(CXXOperatorCallExpr *co = dyn_cast<CXXOperatorCallExpr>(ce->getImplicitObjectArgument()->IgnoreImpCasts())) {
				FindArgument fa(co->getArg(0)->IgnoreImpCasts());
				socketName = fa.getArgumentName();
			}
			FindArgument fa(ce->getArg(0)->IgnoreImpCasts());
			payloadName = fa.getArgumentName();

			FindArgument fb(ce->getArg(1)->IgnoreImpCasts());
			delay = fb.getArgumentName();
		
			Properties *p = new Properties(transportType, payloadName, delay);
			_socketTransportMap.insert(socketTransportPairType(socketName, p));
	}

	else if (ce->getMethodDecl()->getNameAsString() == "nb_transport_fw" || 
	ce->getMethodDecl()->getNameAsString() == "nb_transport_bw") {
	//	_os <<"\n Some dump \n";
	//	ce->dump();
		transportType = ce->getMethodDecl()->getNameAsString();
		if(CXXOperatorCallExpr *co = dyn_cast<CXXOperatorCallExpr>(ce->getImplicitObjectArgument()->IgnoreImpCasts())) {
			FindArgument fa(co->getArg(0)->IgnoreImpCasts());
			socketName = fa.getArgumentName();
		}
		FindArgument fa(ce->getArg(0)->IgnoreImpCasts());
		payloadName = fa.getArgumentName();

		FindArgument fb(ce->getArg(1)->IgnoreImpCasts());
		phase = fb.getArgumentName();

		FindArgument fc(ce->getArg(2)->IgnoreImpCasts());
		delay = fc.getArgumentName();

		Properties *p = new Properties(transportType, payloadName, phase, delay);
		_socketTransportMap.insert(socketTransportPairType(socketName, p));
	}

	return true;
}

FindSocketTransportType::socketTransportMapType FindSocketTransportType::returnSocketTransportMap() {
	return _socketTransportMap;
}

void FindSocketTransportType::dump() {
	_os <<"\n For CXXMethodDecl : " <<_d->getNameAsString();	
	for (socketTransportMapType::iterator it = _socketTransportMap.begin(), eit = _socketTransportMap.end();
	it != eit;
	it++) {
		_os <<"\n Socket : " <<it->first <<" Interface details: ";
		it->second->dump(_os);
	}
}
