#include "FindSocketBinding.h"

using namespace scpar;

FindSocketBinding::FindSocketBinding(CXXRecordDecl *d, llvm::raw_ostream &os)
 : _os(os),
 	_d(d),  
  _constructorStmt(NULL),
	_entryCXXRecordDecl(NULL),
	_entryMethodDecl(NULL),
  pass(1) {

  /// Pass 1:
  /// Find the constructor definition, and the Stmt* that has the code for it.
  /// Set the _constructorStmt pointer.
  //  _os << "\n>>>> PASS 1\n";
  TraverseDecl(_d);
  //  _os << "\n EntryFunctions found: " << _entryFunctions.size() << "\n";
  pass = 2;

  //    _os << "\n>>>> PASS 2\n";
  /// Pass 2:
  /// Get the entry function name from constructor
  TraverseStmt(_constructorStmt);
  pass = 3;
  
  /// Pass 3:
  /// Find the CXXMethodDecl* to the entry function
  TraverseDecl(_d);
  pass = 4;

}
bool FindSocketBinding::VisitCXXMemberCallExpr(CXXMemberCallExpr *ce) {

	switch (pass) {
		case 2:{
	  	if(ce->getMethodDecl()->getNameAsString() == "bind"){				
		
		    if(MemberExpr *me = dyn_cast<MemberExpr>(ce->getImplicitObjectArgument()->IgnoreImpCasts())) {
        	_bindObject = me->getMemberNameInfo().getAsString();

          if(MemberExpr *sme = dyn_cast<MemberExpr>(me->getBase()->IgnoreImpCasts())) {
            _initiator = sme->getMemberNameInfo().getAsString();
          }
        }
        if(MemberExpr *me = dyn_cast<MemberExpr>(ce->getArg(0)->IgnoreImpCasts())) {
          if(MemberExpr *sme = dyn_cast<MemberExpr>(me->getBase()->IgnoreImpCasts())) {
          _target = sme->getMemberNameInfo().getAsString();
          }
        }
        _initiatorTargetSocketMap.insert(initiatorTargetSocketPairType(initiatorTargetPairType(_initiator, 
_target), _bindObject));
      
			}				
		}

	}
	return true;
}

bool FindSocketBinding::VisitCXXMethodDecl(CXXMethodDecl *md) {
  //  _os << "\nVisitCXXMethodDecl: " << pass << ", " << md->getNameAsString() << "\n";  
  switch(pass) {
  	case 1: {
   	 //    _os << "\n\nPass 1 of VisitCXXMethodDecl\n\n";
   	 /// Check if method is a constructor
   	 if (CXXConstructorDecl* cd = dyn_cast<CXXConstructorDecl>(md)) {
   	   const FunctionDecl* fd = NULL;
   	   cd->getBody(fd);
	
 	     if (cd->hasBody()) {
 	 			_constructorStmt = cd->getBody();
 	     }
 	   }
 	   break;
 	 }
 	 case 2: {
 	   break;
 	 }
 	 case 3: {
	
 	   break;
 	 }
  }
  return true;

}
FindSocketBinding::initiatorTargetSocketMapType 
FindSocketBinding::getInitiatorTargetSocketMap() {
	return _initiatorTargetSocketMap;
}


void FindSocketBinding::dump() {
	for (initiatorTargetSocketMapType::iterator it = 
	_initiatorTargetSocketMap.begin(),
	eit = _initiatorTargetSocketMap.end();
	it != eit;
	it++) {
		
		initiatorTargetPairType tmp = it->first;
		_os <<"\n Initiator : " <<tmp.first<<" Socket : " <<it->second<<" Target: "<<tmp.second;  
	}
}

