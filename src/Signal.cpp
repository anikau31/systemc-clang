#include "Signal.h"
#include <string>
#include "FindTemplateTypes.h"

using namespace scpar;
using namespace std;

void Signal::check() { assert(!(signal_container_ == nullptr)); }

Signal::~Signal() {
  if (signal_container_) {
    delete signal_container_;
    signal_container_ = nullptr;
  }
}

Signal::Signal() : signal_name_("NONE"), signal_container_(nullptr) {}

Signal::Signal(const string &name, SignalContainer *s)
    : signal_name_(name), signal_container_(s) {}

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
  // for (int i = 0; i < tabn; i++) {
  //   os << " ";
  // }
  // os << "Signal " << this << " '" << signal_name_ << "' FindTemplateTypes "
  //    << signal_container_->getTemplateTypes() << "' FieldDecl' " <<
  //    signal_container_->getASTNode();
  // signal_container_->getTemplateTypes()->printTemplateArguments(os);
  // dump_json();
}

json Signal::dump_json(raw_ostream &os) {
  json signal_j;
  signal_j["signal_name"] = getName();

  // Container
  auto template_args{
      signal_container_->getTemplateTypes()->getTemplateArgumentsType()};

  signal_j["signal_type"] = template_args[0].getTypeName();
  template_args.erase(begin(template_args));

  for (auto ait = begin(template_args); ait != end(template_args); ++ait) {
    signal_j["signal_arguments"].push_back(ait->getTypeName());
  }

  // os << signal_j.dump(4);

  return signal_j;
}
