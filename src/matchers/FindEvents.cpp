#include "FindEvents.h"
#include "FindTemplateTypes.h"

#include "clang/AST/DeclCXX.h"

using namespace systemc_clang;
//using namespace std;

FindEvents::FindEvents(clang::CXXRecordDecl *d, llvm::raw_ostream &os) : os_(os) {
  TraverseDecl(d);
}

FindEvents::FindEvents(const FindEvents &from) : os_(from.os_) {
  _inClassEvents = from._inClassEvents;
}

FindEvents::~FindEvents() {
  // These pointer are NOT to be deleted
  // : FieldDecl in _inClassEvents.
  // This is because they are pointers to the clang AST, which clang should
  // free.
}

bool FindEvents::VisitFieldDecl(clang::FieldDecl *fd) {
  // FindTemplateTypes te(os_);
  clang::QualType q = fd->getType();
  if (q.getAsString() == "class sc_core::sc_event") {
    if (clang::IdentifierInfo *info = fd->getIdentifier()) {
      // os_ << "\n+ Name: " << info->getNameStart();
      // os_ << "\n+ Type: " << q.getAsString();
      _inClassEvents.insert(kvType(info->getNameStart(), fd));
    }
  }
  return true;
}

FindEvents::classEventMapType FindEvents::getInClassEvents() {
  // FIXME: change to return data structure
  return _inClassEvents;
}

std::vector<std::string> FindEvents::getEventNames() {
  std::vector<std::string> keys;
  for (classEventMapType::iterator vit = begin(_inClassEvents);
       vit != end(_inClassEvents); ++vit) {
    keys.push_back(vit->first);
  }
  return keys;
}

void FindEvents::dump() {
  os_ << "\n ============== FindEvents  ===============";
  os_ << "\n:> Print in-class sc_event data members";
  for (classEventMapType::iterator vit = _inClassEvents.begin();
       vit != _inClassEvents.end(); vit++) {
    os_ << "\n:> name: " << vit->first << ", FieldDecl*: " << vit->second;
  }
  os_ << "\n ============== END FindEvents ===============";
}
