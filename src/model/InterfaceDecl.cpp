#include "InterfaceDecl.h"
#include "FindTemplateTypes.h"

using namespace systemc_clang;

InterfaceDecl::~InterfaceDecl() {
  if (template_type_ != nullptr) {
    delete template_type_;
  }
}

InterfaceDecl::InterfaceDecl() : name_{"NONE"}, template_type_{nullptr} {}

InterfaceDecl::InterfaceDecl(const std::string &name, FindTemplateTypes *tt)
    : name_{name}, template_type_{tt} {}

InterfaceDecl::InterfaceDecl(const InterfaceDecl &from) {
  name_ = from.name_;
  // This is necessary to allow FindInterfaces to go out of scope.
  template_type_ = new FindTemplateTypes(*from.template_type_);
}

void InterfaceDecl::setModuleName(const std::string &name) { name_ = name; }

std::string InterfaceDecl::getName() { return name_; }

FindTemplateTypes *InterfaceDecl::getTemplateType() { return template_type_; }

void InterfaceDecl::dump(llvm::raw_ostream &os, int tabn) {
  for (int i = 0; i < tabn; i++) {
    os << " ";
  }
  os << "InterfaceDecl " << this << " '" << name_ << "' FindTemplateTypes "
     << template_type_;
  template_type_->printTemplateArguments(os);
}
