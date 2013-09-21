#include "FindPortBinding.h"
#include "PortBindContainer.h"

using namespace scpar;

FindMemberExpr::FindMemberExpr(Expr* e, llvm::raw_ostream& os) 
  : _os (os)
  , _name("NONE")
  , _moduleInstanceName("NONE") {
  
  TraverseStmt(e);
}

bool FindMemberExpr::VisitMemberExpr(MemberExpr* e ) {
//  e->dump();
  DeclarationNameInfo di = e->getMemberNameInfo();
  _name = di.getAsString();

  // For X.f, getBase returns X.  
  Expr* base = e->getBase();

  
  if (DeclRefExpr* de = cast<DeclRefExpr>(base)) {
    DeclarationNameInfo di = de->getNameInfo();
    _moduleInstanceName = di.getAsString();
    
  }
  return true;
}

string FindMemberExpr::getName() {
  return _name;
}

string FindMemberExpr::getModuleInstanceName() {
  return _moduleInstanceName;
}

/////////////////////////////////////////////////////////////////////////////////////////////

FindDeclRefExpr::FindDeclRefExpr(Expr* e, llvm::raw_ostream& os)
: _os (os)
, _name("NONE") {

  TraverseStmt(e);
}

bool FindDeclRefExpr::VisitDeclRefExpr(DeclRefExpr* e) {
//  _os << "\n =============== VisitDeclRefExpr =============== \n";
//  e->dump();
  DeclarationNameInfo di = e->getNameInfo();
  _name = di.getAsString();
  return true;
}

string FindDeclRefExpr::getName() {
  return _name;
}

/////////////////////////////////////////////////////////////////////////////////////////////
FindPortBinding::~FindPortBinding() {
//  _os << "[[ Destructor FindPortBinding ]]\n";
//  for (FindPortBinding::signalMapType::iterator sit = _signals->begin();
//       sit != _signals->end();
//       sit++) {
//    delete sit->second;
//  }
//  _signals->clear();
//  delete _signals;
}

FindPortBinding::FindPortBinding(FunctionDecl* d, llvm::raw_ostream &os)
: _os(os)
, _state(EMPTY){

  TraverseDecl(d);
}


bool FindPortBinding::VisitCXXOperatorCallExpr(CXXOperatorCallExpr* e) {

  const Expr* arg0 = e->getArg(0);
  //  _os << "\nDUMP 0 ARG Expr\n";
  
  FindMemberExpr me(const_cast<Expr*>(arg0), _os);
  

  const Expr* arg1 = e->getArg(1);
  //  _os << "\nDUMP 1 ARG Expr\n";
  
  FindDeclRefExpr fe(const_cast<Expr*>(arg1), _os);
  
  PortBindContainer* pb = new PortBindContainer(me.getName(), me.getModuleInstanceName(), fe.getName(), e);
  // Key is the module name?
  _portBinds.insert(FindPortBinding::portBindPairType(me.getModuleInstanceName(), pb));
  _state = CXXOPERATORCALLEXPR;
  return true;
}


FindPortBinding::portBindMapType FindPortBinding::getPortBinds() {
  return _portBinds;
}

void FindPortBinding::dump(raw_ostream& os) {
  _os << "\n================= Find Signals  ================\n";
  for (FindPortBinding::portBindMapType::iterator it = _portBinds.begin(), eit = _portBinds.end();
       it != eit;
       it++) {

    it->second->dump(os, 0);
  }
  os << "\n================= END Find Ports ================\n\n";
}
