#include "PortDecl.h"
#include "FindTemplateTypes.h"

using namespace scpar;
using namespace std;

PortDecl::~PortDecl() {
  if ( template_type_ != nullptr ) {
    delete template_type_;
    template_type_ = nullptr;
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
  //os << "Port name: " << port_name_ << " ";
  //template_type_->printTemplateArguments(os);

  dump_json();
}

json PortDecl::dump_json() {

  json port_j;
  port_j["port_name"] = getName();

  // Template arguments
  auto template_args{ template_type_->getTemplateArgumentsType() };
  port_j["port_type"] = template_args[0].getTypeName();
  template_args.erase( begin(template_args) );

  for ( auto ait = begin(template_args); ait != end(template_args); ++ait ) {
    port_j["port_arguments"].push_back( ait->getTypeName() );
  }

  std::cout << port_j.dump(4);
  return port_j;
}
