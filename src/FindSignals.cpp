#include "FindSignals.h"
#include "FindTemplateTypes.h"

using namespace scpar;

FindSignals::~FindSignals() {
  for ( auto const & sit : signalcontainer_map_ ) {
    delete sit.second;
  }

  signalcontainer_map_.clear();
}

FindSignals::FindSignals(CXXRecordDecl *d, llvm::raw_ostream &os) : os_(os) {
  TraverseDecl(d);
}

bool FindSignals::VisitFieldDecl(FieldDecl *fd) {
  QualType q {fd->getType() };

  if (IdentifierInfo *info = fd->getIdentifier()) {
    /// We are going to store these.  So use pointers.
    const Type *tp{ q.getTypePtr() };
    FindTemplateTypes *te { new FindTemplateTypes() };

    te->Enumerate(tp);
    string tt{ te->getTemplateType() };

    // If string is not found
    if ( tt.find( "sc_signal") == string::npos ) {
      delete te;
      return true;
    }

    SignalContainer *sc = new SignalContainer(fd->getNameAsString(), te, fd);
    signalcontainer_map_.insert(FindSignals::signalPairType(fd->getNameAsString(), sc));
  }
  return true;
}

FindSignals::signalMapType FindSignals::getSignals() const { return signalcontainer_map_; }

void FindSignals::dump() {
  os_ << "\n================= Find Signals  ================\n";
  for (auto const & sit : signalcontainer_map_ ) {
    os_ << sit.second;
    sit.second->dump(os_);
  }
  os_ << "\n================= END Find Ports ================\n\n";
}
