#include "FindGlobalEvents.h"
using namespace scpar;

FindGlobalEvents::FindGlobalEvents(TranslationUnitDecl * d, llvm::raw_ostream & os):
_os
  (os)
{
  TraverseDecl(d);
}

FindGlobalEvents::~FindGlobalEvents() {
}

bool FindGlobalEvents::VisitVarDecl(VarDecl * vd)
{
  QualType q = vd->getType();
  
  if (q.getAsString() == "class sc_core::sc_event") {
    if (IdentifierInfo * info = vd->getIdentifier()) {

      _globalEvents.insert(kvType(info->getNameStart(), vd));

    }
  }
  return true;
}

FindGlobalEvents::globalEventMapType FindGlobalEvents::getEventMap()
{
  return _globalEvents;
}

void FindGlobalEvents::dump()
{
  _os << "\n ==============  FindGlobalEvents ===============";
  _os << "\n:> Print global sc_event variables";
  for (globalEventMapType::iterator vit = _globalEvents.begin();
       vit != _globalEvents.end(); vit++) {
    _os << "\n Name: " << vit->first << ", VarDecl*: " << vit->second;
  }
  _os << "\n ============== END FindGlobalEvents ===============";
}
