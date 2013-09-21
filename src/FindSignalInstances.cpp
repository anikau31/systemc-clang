#include "FindSignalInstances.h"
#include "FindTemplateTypes.h"
#include "SignalInstanceContainer.h"

using namespace scpar;
using namespace std;

FindSignalInstances::FindSignalInstances(FunctionDecl* d)
: _state(EMPTY)
, _tempName("NONE") {
  TraverseDecl(d);
}

FindSignalInstances::FindSignalInstances(const FindSignalInstances& from) {
  _signalInstances = from._signalInstances;
}

FindSignalInstances::~FindSignalInstances() {

  _signalInstances.clear();
}

FindSignalInstances::signalInstanceMapType FindSignalInstances::getSignalInstances() {
  return _signalInstances;
}

bool FindSignalInstances::VisitDeclStmt(DeclStmt* s) {
//  llvm::errs() << " >>>>>>>>>>>>>>>>>>> FindSignalInstances DECL STMT <<<<<<<<<<<<<<<<<<<<<<< \n";
  _state = DECLSTMT;
 
  if (s->isSingleDecl()) {
    Decl* d = s->getSingleDecl(); 
    if (NamedDecl *nd = dyn_cast<NamedDecl>(d)) {
//      llvm::errs() << "Named decl: " << nd->getNameAsString() << "\n";
      _tempName = nd->getNameAsString();

    }
  }
  return true;
}


bool FindSignalInstances::VisitCXXConstructExpr(CXXConstructExpr* e) {
  
 if (_state != DECLSTMT) {
    return true;
  }

  QualType q = e->getType();

  // Check that the base class name is sc_signal
  if (const IdentifierInfo* id = q.getBaseTypeIdentifier()) {
//    llvm::errs() << " base name: " << id->getNameStart() << "\n";
    string base(id->getNameStart());
    if (base != "sc_signal") {
  //    llvm::errs() << "This is NOT a signal\n";
      return true;
    }
  }

  FindTemplateTypes* tt = new FindTemplateTypes();
  tt->Enumerate(q.getTypePtr());
  string n = q.getAsString();

  SignalInstanceContainer* sc = new SignalInstanceContainer(_tempName,n, tt, e);
  _signalInstances.insert(signalInstancePairType(_tempName, sc));
  _state = EMPTY;

  return true;
}


void FindSignalInstances::dump(raw_ostream& os, int tabn) {
  os << "\n ============== FindSignalInstances  ===============\n";
  for (FindSignalInstances::signalInstanceMapType::iterator mit = _signalInstances.begin();
       mit != _signalInstances.end();
       mit++) {

    for (int i = 0; i <tabn; i++) {
      os << " ";
    }
    
    os << "FindSignalInstances '" << mit->first << "' SignalInstanceContainer " << mit->second << "\n";
    mit->second->dump(os, ++tabn);
  }
  os << "\n ============== END FindSignalInstances ===============";  
}
