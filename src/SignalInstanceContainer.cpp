#include "SignalInstanceContainer.h"

using namespace scpar;
using namespace std;

SignalInstanceContainer::SignalInstanceContainer()
: _name("NONE")
, _signalType("NONE")
, _templateType(NULL)
, _astNode(NULL)  {

}

SignalInstanceContainer::SignalInstanceContainer(string n, string t, FindTemplateTypes* tt, CXXConstructExpr* d)
: _name(n)
, _signalType(t)
, _templateType(tt)
, _astNode(d)  {

}


SignalInstanceContainer::SignalInstanceContainer(const SignalInstanceContainer& from) {
  _name = from._name;
  _signalType = from._signalType;
  _templateType = from._templateType;
  _astNode = from._astNode;
}

SignalInstanceContainer::~SignalInstanceContainer() {
  // _tempalteTypes is allocated during FindSignalInstances.
  // These are stord in SignalInstanceContainer and have to be deleted.
  // Do not delete the following: _astNode (freed by clang)
  if (_templateType != NULL) {
    delete _templateType;
  }
}

string SignalInstanceContainer::getName() {
  return _name;
}

string SignalInstanceContainer::getType() {
  return _signalType;
}

FindTemplateTypes* SignalInstanceContainer::getTemplateTypes() {
  return _templateType;
}

CXXConstructExpr* SignalInstanceContainer::getASTNode() {
  return _astNode;
}

void SignalInstanceContainer::dump(raw_ostream& os, int tabn) {
  os << " SignalInstanceContainer '" << _name << "' Type: '" << _signalType << "'  CXXConstructExpr " << _astNode << "\n";
  _templateType->printTemplateArguments(os, ++tabn);
}
