#include "SocketBindGen.h"
using namespace scpar;

SocketBindGen::SocketBindGen (FunctionDecl * fnDecl, llvm::raw_ostream & os):
_os (os),
pass(1)
{

  TraverseDecl (fnDecl);
}

SocketBindGen::~SocketBindGen ()
{

}

bool SocketBindGen::VisitMemberExpr (MemberExpr * me)
{
  
  string tmpType = me->getType ().getBaseTypeIdentifier ()->getName ();

  if(tmpType == "simple_initiator_socket" || tmpType == "simple_target_socket" 
	|| tmpType == "tlm_initiator_socket" || tmpType == 
	"tlm_target_socket" 
	|| tmpType == "simple_initiator_socket_tagged" || tmpType == 
	"simple_target_socket_tagged" ||  tmpType == 
	"passthrough_target_socket_tagged" || tmpType == "passthrough_target_socket" 
	|| tmpType == "multi_passthrough_initiator_socket"|| tmpType == 
	"multi_passthrough_target_socket") {
		
		if(pass%2 == 1) {
			_initiatorSocket = me->getMemberNameInfo ().getAsString ();		
		}
		else {
			_targetSocket = me->getMemberNameInfo ().getAsString ();
			_initiatorTargetSocketMap.insert(initiatorTargetSocketPairType(_initiatorSocket, 
			_targetSocket));
		}
		
  	Expr *e = me->getBase ();

  	if (DeclRefExpr * de = dyn_cast < DeclRefExpr > (e)) {
			if(pass%2 == 1) {				
    		_initiatorInstance = de->getNameInfo ().getAsString ();
    		_initiatorModule = de->getDecl()->getType().getTypePtr()->getAsCXXRecordDecl()->getIdentifier()->getNameStart();				
				_initiatorTargetInstanceMap.insert(initiatorTargetInstancePairType(_initiatorInstance, 
				_initiatorModule));
			}
			else {
		 		_targetInstance = de->getNameInfo ().getAsString ();
    		_targetModule = de->getDecl()->getType().getTypePtr()->getAsCXXRecordDecl()->getIdentifier()->getNameStart();		
				_initiatorTargetInstanceMap.insert(initiatorTargetInstancePairType(_targetInstance, 
				_targetModule));
				_initiatorTargetMap.insert(initiatorTargetPairType(_initiatorInstance, 
				_targetInstance));

				_bindMap.insert(bindPairType(initiatorTargetSocketPairType(_initiatorSocket, 
				_targetSocket), initiatorTargetPairType(_initiatorInstance, 
				_targetInstance)));
			}				
  	}
		pass = pass + 1;
	}
  return true;
}
  
void SocketBindGen::dump ()
{
  _os << "\n ======================= Begin Socket bindings ====================";

	for (bindMapType::iterator it = _bindMap.begin(), eit = _bindMap.end();
	it != eit;
	it++) {
		initiatorTargetSocketPairType tmpInitiatorTargetSocketPair = it->first;
		initiatorTargetPairType tmpInitiatorTargetPair = it->second;

		_os <<"\n Initiator : " <<tmpInitiatorTargetSocketPair.first<<" Initiator Socket : " << tmpInitiatorTargetPair.first<<"\n";
		_os <<"\n Target : " <<tmpInitiatorTargetSocketPair.second<<" Target Socket : " 
		<<tmpInitiatorTargetPair.second<<"\n";
	}

  _os << "\n ======================= End Socket Bindings ====================";  
}
