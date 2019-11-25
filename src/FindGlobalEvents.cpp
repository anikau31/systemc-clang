#include "FindGlobalEvents.h"
using namespace scpar;

FindGlobalEvents::FindGlobalEvents(TranslationUnitDecl *declaration,
                                   llvm::raw_ostream &os)
    : _os(os) {
  TraverseDecl(declaration);
}

FindGlobalEvents::~FindGlobalEvents() {}

bool FindGlobalEvents::VisitVarDecl(VarDecl *variable_declaration) {
  QualType variable_type{variable_declaration->getType()};

  if (variable_type.getAsString() == "class sc_core::sc_event") {
    if (IdentifierInfo *info = variable_declaration->getIdentifier()) {
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

json FindGlobalEvents::dump_json() {
  json globals_j{};

  for (auto const &event : _globalEvents) {
    globals_j["global_declarations"].emplace_back(event.first);
  }
  std::cout << globals_j.dump(4) << endl;
  return globals_j;
}
