#include "InterfaceDecl.h"
#include <string>
#include "FindTemplateTypes.h"
using namespace systemc_clang;
using namespace std;

InterfaceDecl::~InterfaceDecl() {
  if (_templateType != nullptr) {
    delete _templateType;
  }
}

InterfaceDecl::InterfaceDecl() : _name{"NONE"}, _templateType{nullptr} {}

InterfaceDecl::InterfaceDecl(const string &name, FindTemplateTypes *tt)
    : _name{name}, _templateType{tt} {}

InterfaceDecl::InterfaceDecl(const InterfaceDecl &from) {
  _name = from._name;
  // This is necessary to allow FindInterfaces to go out of scope.
  _templateType = new FindTemplateTypes(*from._templateType);
}

void InterfaceDecl::setModuleName(const string &name) { _name = name; }

string InterfaceDecl::getName() { return _name; }

FindTemplateTypes *InterfaceDecl::getTemplateType() { return _templateType; }

void InterfaceDecl::dump(raw_ostream &os, int tabn) {
  for (int i = 0; i < tabn; i++) {
    os << " ";
  }
  os << "InterfaceDecl " << this << " '" << _name << "' FindTemplateTypes "
     << _templateType;
  _templateType->printTemplateArguments(os);
}
