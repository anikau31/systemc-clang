#include "FindPortReadWrites.h"
#include "FindTemplateTypes.h"

using namespace scpar;
using namespace std;

FindPortReadWrites::FindPortReadWrites(CXXMethodDecl* d, llvm::raw_ostream &os):_os(os) {
  // Initialize the internal FSM.
  _state = 0;
  _rw = RWINIT;
  // Traverse the declaration's AST.
  TraverseDecl(d);

}

bool FindPortReadWrites::VisitMemberExpr(MemberExpr* e) {
  QualType q = e->getType();
  ValueDecl* vd = e->getMemberDecl();

  if (vd == NULL)
    return true;
  
  string name(vd->getNameAsString());
  
  // Skip out if the member expression is on an event type.
  // Particularly, a call wait/notify.
  if (name == "wait" || name == "notify")
    return true;
  
  switch(_state) {
  case 0: {
    // Currently looking for read/write on ports.

    if (name == "read") {
      //      _os << "## found a read\n";
    _rw = READ;
    _state = 1;    
    } else
    if (name == "write") {    
      //   _os << "## found a write\n";
    _rw = WRITE;
    _state = 1;    
    }
    break;
  }
  case 1:
    {
    // Try to find the name of the variable upon which read/write was
    // called.
      switch (_rw) {
      case READ:
	_portReads.insert(kvType(name,e));
	break;
      case WRITE:
	_portWrites.insert(kvType(name,e));      
	break;
      };
    _state = 0;
      _rw = RWINIT;
    break;
    }

  };
  return true;
}
string FindPortReadWrites::getVariableName(Expr* e) {

  clang::LangOptions LangOpts;
  LangOpts.CPlusPlus = true;
  clang::PrintingPolicy Policy(LangOpts);
  
  string TypeS;
  llvm::raw_string_ostream str(TypeS);

  e->printPretty(str, 0, Policy);
  //_os << ", argument: " << str.str();
  return str.str();

}

string FindPortReadWrites::getDataMembertype(Expr* fd) {
  QualType q = fd->getType();
  return q.getAsString();
}

void FindPortReadWrites::dump() {
  _os << "\n ============== FindPortReadWrites  ===============";
  _os << "\n:> Port reads ";
  for(classMemberMapType::iterator vit = _portReads.begin(); vit!= _portReads.end(); vit++) {
    _os << "\n- name: " << vit->first << ", type: " << getDataMembertype(vit->second) << ", Expr*: " << vit->second ;
  }

  _os << "\n\n:> Port Writes";
  for(classMemberMapType::iterator vit = _portWrites.begin(); 
      vit!= _portWrites.end(); vit++) {
    _os << "\n- name: " << vit->first << ", type: " << getDataMembertype(vit->second) << ", Expr*: " << vit->second ;
   }
 
  _os << "\n ============== END FindPortReadWrites ===============";  
}
