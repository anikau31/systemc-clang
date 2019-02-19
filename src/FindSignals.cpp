#include "FindSignals.h"
#include "FindTemplateTypes.h"

using namespace scpar;

FindSignals::~FindSignals() {
  //  _os << "[[ Destructor FindSignals ]]\n";
  //for (FindSignals::signalMapType::iterator sit = signalcontainer_map_->begin();
  //sit != signalcontainer_map_->end(); sit++) {
  for ( auto const & sit : signalcontainer_map_ ) {
    delete sit.second;
  }

  signalcontainer_map_.clear();
  //delete signalcontainer_map_;
}

FindSignals::FindSignals(CXXRecordDecl *d, llvm::raw_ostream &os) : _os(os) {
  //signalcontainer_map_ = new FindSignals::signalMapType();
  TraverseDecl(d);
}

bool FindSignals::VisitFieldDecl(FieldDecl *fd) {
  QualType q = fd->getType();

  if (IdentifierInfo *info = fd->getIdentifier()) {
    //    fname = info->getNameStart();
    //    _os << "\n+ Name: " << info->getNameStart();
    //    _os << "\n+ Type: " << q.getAsString();
    //   _os << "\n+ also name: " << fd->getNameAsString();

    /// We are going to store these.  So use pointers.
    const Type *tp = q.getTypePtr();
    FindTemplateTypes *te = new FindTemplateTypes();

    te->Enumerate(tp);
    // te->printTemplateArguments(_os);

    string tt = te->getTemplateType();

    //    _os << "OUTPUT ============ " << tt << "\n";
    if ((signed)tt.find("sc_signal") == -1) {
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
  _os << "\n================= Find Signals  ================\n";

  //for (FindSignals::signalMapType::iterator sit = signalcontainer_map_->begin();
  //sit != signalcontainer_map_->end(); sit++) {
  for (auto const & sit : signalcontainer_map_ ) {
    _os << sit.second;
    sit.second->dump(_os);
  }
  _os << "\n================= END Find Ports ================\n\n";
}
