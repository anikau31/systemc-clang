#include "FindSocketRegisterMethods.h"

using namespace scpar;

FindSocketRegisterMethods::FindSocketRegisterMethods(CXXRecordDecl *d, llvm::raw_ostream &os)
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

FindSocketRegisterMethods::~FindSocketRegisterMethods() {
	_callbackMethods.clear();
}

bool FindSocketRegisterMethods::VisitCXXMemberCallExpr(CXXMemberCallExpr *ce) {
	
	switch (pass) {
		case 2:{

			if((ce->getMethodDecl()->getNameAsString() == "register_b_transport") || (ce->getMethodDecl()->getNameAsString() == "register_nb_transport_fw") || (ce->getMethodDecl()->getNameAsString() == "register_get_direct_mem_ptr") || (ce->getMethodDecl()->getNameAsString() == "register_transport_dbg") || (ce->getMethodDecl()->getNameAsString() == "register_nb_transport_bw") || (ce->getMethodDecl()->getNameAsString() == "register_invalidate_direct_mem_ptr")) {
				_callbackMethods.push_back(ce->getMethodDecl()->getNameAsString());
			}		
		}
	}
	return true;
}

bool FindSocketRegisterMethods::VisitCXXMethodDecl(CXXMethodDecl *md) {
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

FindSocketRegisterMethods::callbackMethods FindSocketRegisterMethods::getCallbackMethods() {
	return _callbackMethods;
}
