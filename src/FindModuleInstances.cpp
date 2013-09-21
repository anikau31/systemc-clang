#include "FindModuleInstances.h"
#include "Utility.h"

using namespace scpar;
using namespace std;

FindModuleInstances::FindModuleInstances(FunctionDecl* d)
: _state(EMPTY) {
  
  TraverseDecl(d);
  
}

bool FindModuleInstances::isState(ASTSTATE s) {
  return _state == s;
} 

FindModuleInstances::FindModuleInstances(const FindModuleInstances& from) {
  // Copy constructor works for pointers.
   _moduleInstances = from._moduleInstances;
}

FindModuleInstances::~FindModuleInstances() {
  // We are storing pointers to ModuleInstanceContainer.  
  // We have to free them before clearing the map.

  for (moduleInstanceContainerMapType::iterator it = _moduleInstances.begin();
      it != _moduleInstances.end();
      it++) {

    delete it->second;
  }
  _moduleInstances.clear();
}

bool FindModuleInstances::VisitDeclStmt(DeclStmt* s) {
 // llvm::errs() << " >>>>>>>>>>>>>>>>>>> FindModuleInstances DECL STMT <<<<<<<<<<<<<<<<<<<<<<< \n";

  _state = DECLSTMT;
  return true;
}
//
//string FindModuleInstances::strip(string s, string sub) {
//    // sub has "struct "
//  size_t pos = s.find(sub);
//  if (pos == string::npos) {
//    return s;
//  }
//  
//  return s.erase(pos, sub.length());
//}

bool FindModuleInstances::VisitCXXConstructExpr(CXXConstructExpr* e) {

  if (!isState(DECLSTMT)) {
    return true;
  }
  
  //e->dump();
//  llvm::errs() << "Enter CXXConstructExpr\n";

  QualType q = e->getType();
  string n = q.getAsString();
  
  clang::LangOptions LangOpts;
  LangOpts.CPlusPlus = true;
  clang::PrintingPolicy Policy(LangOpts);
  string TypeS;
  llvm::raw_string_ostream str(TypeS);
  e->printPretty(str, 0, Policy);
  
  Utility u;
  string modtype = u.strip(n,"struct ");
  
//  // Check if the base class is an sc_module.
//  bool hasBase = false;
//  CXXConstructorDecl* cd = e->getConstructor();
//  for (CXXConstructorDecl::init_iterator ii = cd->init_begin(), iie = cd->init_end();
//       ii != iie; 
//       ++ii) {
//      //QualType q = ii->getType();
//    const Type* bt = (*ii)->getBaseClass();
  // if (bt != NULL) {
//    string baseName(bt->getTypeClassName());
//    llvm::errs() << "BAE NAME: " << baseName << "\n";
//    }
////      if (baseName == "::sc_core::sc_module" || baseName == "sc_core::sc_module") {
////        hasBase = true;
////      }
//  }
  
  // Check if the current class is not a constructor
  if ((modtype == n)) {
    return true;
  }
  
  // Have to check if it a member of the modules found first.
  
//  llvm::errs() << modtype << ", modname: " << str.str() << "\n";
  ModuleInstanceContainer* mc = new ModuleInstanceContainer(str.str(), modtype, e );
  _moduleInstances.insert(moduleInstanceContainerPairType(str.str(), mc) );
  _state = EMPTY;
  
  return true;
}

void FindModuleInstances::dump(raw_ostream& os, int tabn) {
  os << "\n ============== FindModuleInstances  ===============\n";
  for (moduleInstanceContainerMapType::iterator it = _moduleInstances.begin();
       it != _moduleInstances.end();
       it++) {
    
    for (int i = 0; i <tabn; i++) {
      os << " ";
    }
    os << "FindModuleInstances '" << it->first <<" ";
    it->second->dump(os, ++tabn);
  }
  os << "\n ============== END FindModuleInstances ===============\n";  
}
