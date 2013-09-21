#include "FindGlobalVariables.h"
#include "clang/AST/Decl.h"
using namespace scpar;

FindGlobalVariables::FindGlobalVariables(TranslationUnitDecl* d, llvm::raw_ostream &os):
  _os(os) {
  TraverseDecl(d);
}
bool FindGlobalVariables::VisitStmt(Stmt *stmt) {

	return true;
}

bool FindGlobalVariables::VisitVarDecl(VarDecl* vd) {
  QualType q = vd->getType();  
  
	//_os << "\n+ Type: " << q.getAsString();
	if(vd->hasGlobalStorage()) {
		_os <<"\n FOUND GLOBAL \n";
		vd->dump();
	}
  /*
	if (q.getAsString() == "class sc_core::sc_event") {
    if (IdentifierInfo *info = vd->getIdentifier()) {
     //  _os << "\n+ Name: " << info->getNameStart();
      //_os << "\n+ Type: " << q.getAsString();
		
     		_globalEvents.insert(kvType(info->getNameStart(), vd));
      
    }
  }
	*/
  return true;
}

FindGlobalVariables::globalVariableMapType FindGlobalVariables::getVariableMap() {
  return _globalVariables;
}

void FindGlobalVariables::dump() {
  _os << "\n ==============  FindGlobalVariables ===============";  
  _os << "\n:> Print global variables";
  for(globalVariableMapType::iterator vit = _globalVariables.begin(); vit!= _globalVariables.end(); vit++) {
    _os << "\n Name: " << vit->first << ", VarDecl*: " << vit->second ;
  }
  _os << "\n ============== END FindGlobalVariables ===============";
}

