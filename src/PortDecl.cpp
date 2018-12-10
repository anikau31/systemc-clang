//#include <string>
#include "PortDecl.h"
#include "FindTemplateTypes.h"
using namespace scpar;
using namespace std;

PortDecl::~PortDecl() {
  if ( _templateType != nullptr ) {
    delete _templateType;
  }
}

PortDecl::PortDecl():_name{"NONE"}, _templateType{nullptr} {

}

PortDecl::PortDecl(const string &name, FindTemplateTypes *tt):_name{name}, _templateType{tt} {
}

PortDecl::PortDecl(const PortDecl &from) {
  _name = from._name;
  // This is necessary to allow FindPorts to go out of scope.
  _templateType = new FindTemplateTypes(*from._templateType);
}

void PortDecl::setModuleName(const string &name) {
  _name = name;
}

string PortDecl::getName() const {
  return _name;
}

FindTemplateTypes *PortDecl::getTemplateType() {
  return _templateType;
}

void PortDecl::dump(raw_ostream & os, int tabn) {
  for ( auto i = 0; i < tabn; ++i ) {
    os << " ";
  }
  os << "PortDecl " << this << " '" << _name << "' FindTemplateTypes " <<
    _templateType;
  _templateType->printTemplateArguments(os, 1);
}
