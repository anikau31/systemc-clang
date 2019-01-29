//#include <string>
#include "PortDecl.h"
#include "FindTemplateTypes.h"
using namespace scpar;
using namespace std;

PortDecl::~PortDecl() {
  if (template_type_ != nullptr) {
    delete template_type_;
  }
}

PortDecl::PortDecl() : port_name_{"NONE"}, template_type_{nullptr} {}

PortDecl::PortDecl(const string &name, FindTemplateTypes *tt)
    : port_name_{name}, template_type_{tt} {}

PortDecl::PortDecl(const PortDecl &from) {
  port_name_ = from.port_name_;
  // This is necessary to allow FindPorts to go out of scope.
  template_type_ = new FindTemplateTypes{*from.template_type_};
}

void PortDecl::setModuleName(const string &name) { port_name_ = name; }

string PortDecl::getName() const { return port_name_; }

FindTemplateTypes *PortDecl::getTemplateType() { return template_type_; }

void PortDecl::dump(llvm::raw_ostream &os, int tabn) {
  // os << "PortDecl " << this << " '" << port_name_ << "' FindTemplateTypes "
  // << template_type_;
  os << "Port name: " << port_name_ << " ";
  template_type_->printTemplateArguments(os);
}
