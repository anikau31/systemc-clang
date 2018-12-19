#include "SCModules.h"
#include "FindModule.h"
#include "clang/AST/ASTContext.h"
using namespace scpar;

SCModules::SCModules(TranslationUnitDecl *tuDecl, llvm::raw_ostream & os):
_os(os) {
  assert(!(tuDecl == nullptr));
  TraverseDecl(tuDecl);
}

SCModules::~SCModules() {
}

bool SCModules::VisitCXXRecordDecl(CXXRecordDecl *cxxDecl) {
  FindModule mod(cxxDecl, _os);

  if (!mod.isSystemCModule()) {
    return true;
  }
  string modName = mod.getModuleName();
  _moduleMap.insert(modulePairType(modName, cxxDecl));
  return true;
}

SCModules::moduleMapType SCModules::getSystemCModulesMap() {
  return _moduleMap;
}

void SCModules::dump() {
  _os << "\n================= SCModules ================";
  _os << "\n Print SystemC Module Map";
  for (moduleMapType::iterator mit = _moduleMap.begin();
       mit != _moduleMap.end(); mit++) {
    _os << "\n:> name: " << mit->first << ", CXXRecordDecl*: " << mit->second;
  }
  _os << "\n================= END SCModules ================";
  _os << "\n\n";
}
