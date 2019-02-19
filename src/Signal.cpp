#include "Signal.h"
#include "FindTemplateTypes.h"
#include <string>

using namespace scpar;
using namespace std;

void Signal::check() { assert(!(signal_container_ == nullptr)); }

Signal::Signal() : signal_name_("NONE"), signal_container_(nullptr) {}

Signal::Signal(const string &name, SignalContainer *s) : signal_name_(name), signal_container_(s) {}

void Signal::setModuleName(const string &name) { signal_name_ = name; }

string Signal::getName() { return signal_name_; }

FindTemplateTypes *Signal::getTemplateTypes() {
  check();
  return signal_container_->getTemplateTypes();
}

FieldDecl *Signal::getASTNode() {
  check();
  return signal_container_->getASTNode();
}

void Signal::dump(raw_ostream &os, int tabn = 0) {
  check();
  for (int i = 0; i < tabn; i++) {
    os << " ";
  }
  os << "Signal " << this << " '" << signal_name_ << "' FindTemplateTypes "
     << signal_container_->getTemplateTypes() << "' FieldDecl' " << signal_container_->getASTNode();
  signal_container_->getTemplateTypes()->printTemplateArguments(os);
}
