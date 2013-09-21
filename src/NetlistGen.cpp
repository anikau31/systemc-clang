#include "NetlistGen.h"
using namespace scpar;

NetlistGen::NetlistGen (FunctionDecl * fnDecl, llvm::raw_ostream & os):
_os (os)
{

  TraverseDecl (fnDecl);
}

NetlistGen::~NetlistGen ()
{
	_instanceModuleMap.clear();
	_signalPortTypeMap.clear();
	_netlistMap.clear();
	_portName.clear();
	_moduleInstance.clear();
	_moduleName.clear();
	_portType.clear();	
}

bool NetlistGen::VisitMemberExpr (MemberExpr * me)
{
   if(me->getType().getBaseTypeIdentifier()->getName() == "simple_initiator_socket" || me->getType().getBaseTypeIdentifier()->getName() == "simple_target_socket"
  || me->getType().getBaseTypeIdentifier()->getName() == "tlm_initiator_socket" || me->getType().getBaseTypeIdentifier()->getName() ==
  "tlm_target_socket"
  || me->getType().getBaseTypeIdentifier()->getName() == "simple_initiator_socket_tagged" || me->getType().getBaseTypeIdentifier()->getName() ==
  "simple_target_socket_tagged" ||  me->getType().getBaseTypeIdentifier()->getName() ==
  "passthrough_target_socket_tagged" || me->getType().getBaseTypeIdentifier()->getName() == "passthrough_target_socket"
  || me->getType().getBaseTypeIdentifier()->getName() == "multi_passthrough_initiator_socket"|| me->getType().getBaseTypeIdentifier()->getName() ==
  "multi_passthrough_target_socket") {
 		return true;
	}
  _portType = me->getType ().getBaseTypeIdentifier ()->getName ();	
	
	_portName = me->getMemberNameInfo ().getAsString ();

  Expr *e = me->getBase ();

  if (DeclRefExpr * de = dyn_cast < DeclRefExpr > (e)) {

    _moduleInstance = de->getNameInfo ().getAsString ();
    _moduleName = de->getDecl()->getType().getTypePtr()->getAsCXXRecordDecl()->getIdentifier()->getNameStart();

		
		if(_instanceModuleMap.find(_moduleInstance) == _instanceModuleMap.end()) {
				_instanceModuleMap.insert(instanceModulePairType(_moduleInstance, _moduleName));
		} 
  }
  return true;
}

/*
bool NetlistGen::VisitCXXOperatorCallExpr(CXXOperatorCallExpr* ce) {
		
	_os <<"\n CXXOperator call expr dump : \n";
	ce->getCallee()->IgnoreImpCasts()->dump();

	if(DeclRefExpr *de = 
	dyn_cast<DeclRefExpr>(ce->getCallee()->IgnoreImpCasts())) {
		_os <<"GWIBBER : " <<de->getNameInfo().getAsString();
		if(de->getNameInfo().getAsString() == "operator()") {
			pass = 1;	
		}
	}

	return true;
}
*/

bool NetlistGen::VisitImplicitCastExpr (ImplicitCastExpr * ice)
{

  Expr *e = ice->getSubExpr ();
  vector < string > tmpPorts;

  if (DeclRefExpr * de = dyn_cast < DeclRefExpr > (e)) {
    if (_portName.size () != 0) {
      _signalName = de->getNameInfo ().getAsString ();
      if(_signalPortTypeMap.find(signalPortPairType(_signalName, _portName)) == _signalPortTypeMap.end()) {
				_signalPortTypeMap.insert(signalPortTypePairType(signalPortPairType(_signalName, _portName), _portType));
				_netlistMap.insert(netlistPairType(instanceModulePairType(_moduleInstance, _moduleName), signalPortPairType(_signalName, _portName)));
			}
		}	
		_portName.clear ();
  }  
  return true;
}

NetlistGen::instanceModuleMapType NetlistGen::getInstanceModuleMap() {
	return _instanceModuleMap;
}

NetlistGen::signalPortTypeMapType NetlistGen::getSignalPortTypeMap() {
	return _signalPortTypeMap;
}

NetlistGen::netlistMapType NetlistGen::getNetlistMap() {
	return _netlistMap;
}
  
void NetlistGen::dump ()
{
  _os << "\n ======================= Begin netlist ====================";

	for (netlistMapType::iterator it = _netlistMap.begin(), eit = _netlistMap.end();
				it != eit;
				it++) {
		instanceModulePairType tmpInstanceModulePair = it->first;
		signalPortPairType tmpSignalPortPair = it->second;
		
		_os <<"\n Module instance : " <<it->first.first<<" Module Name : " <<it->first.second<< " Signal Name : " <<it->second.first<<" Port Name : " <<it->second.second;
		signalPortTypeMapType::iterator signalFound = _signalPortTypeMap.find(it->second);
		_os <<" Port Type : " <<signalFound->second;
		
	}
  _os << "\n ======================= End netlist ====================";  
}
