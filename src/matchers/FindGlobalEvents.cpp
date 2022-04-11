#include "FindGlobalEvents.h"
#include "clang/AST/Decl.h"

using namespace systemc_clang;

FindGlobalEvents::FindGlobalEvents(clang::TranslationUnitDecl *declaration,
                                   llvm::raw_ostream &os)
    : _os(os) {
  TraverseDecl(declaration);
}

FindGlobalEvents::~FindGlobalEvents() {}

bool FindGlobalEvents::VisitVarDecl(clang::VarDecl *variable_declaration) {
  clang::QualType variable_type{variable_declaration->getType()};

  if (variable_type.getAsString() == "class sc_core::sc_event") {
    if (clang::IdentifierInfo *info = variable_declaration->getIdentifier()) {
      _globalEvents.insert(kvType(info->getNameStart(), variable_declaration));
    }
  }
  return true;
}

FindGlobalEvents::globalEventMapType FindGlobalEvents::getEventMap() {
  return _globalEvents;
}

void FindGlobalEvents::dump() {
  _os << "\n ==============  FindGlobalEvents ===============";
  _os << "\n:> Print global sc_event variables";
  for (globalEventMapType::iterator vit = _globalEvents.begin();
       vit != _globalEvents.end(); vit++) {
    _os << "\n Name: " << vit->first << ", VarDecl*: " << vit->second;
  }
  _os << "\n ============== END FindGlobalEvents ===============";
}

std::string FindGlobalEvents::asString() const {
  std::string str{};

  if (_globalEvents.size() > 0) {
    str += "global_declarations: ";
  }

  for (auto const &event : _globalEvents) {
    str += "  " + event.first;
  }

  str += "\n";
  llvm::outs() << str;
  return str;
}
