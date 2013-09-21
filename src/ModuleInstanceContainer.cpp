#include "ModuleInstanceContainer.h"

using namespace scpar;
using namespace std;

ModuleInstanceContainer::ModuleInstanceContainer()
: _name("NONE")
, _moduleType("NONE")
, _astNode(NULL)  {

}

ModuleInstanceContainer::ModuleInstanceContainer(string n, string t, CXXConstructExpr* d)
: _name(n)
, _moduleType(t)
, _astNode(d)  {

}


ModuleInstanceContainer::ModuleInstanceContainer(const ModuleInstanceContainer& from) {
  _name = from._name;
  _moduleType = from._moduleType;
  _astNode = from._astNode;
}

ModuleInstanceContainer::~ModuleInstanceContainer() {
  // Free nothing.  
  // None of the members allocate memory.

}

string ModuleInstanceContainer::getName() {
  return _name;
}

string ModuleInstanceContainer::getModuleType() {
  return _moduleType;
}

CXXConstructExpr* ModuleInstanceContainer::getASTNode() {
  return _astNode;
}

void ModuleInstanceContainer::dump(raw_ostream& os, int tabn) {
  os << "ModuleInstanceContainer '" << _name << "' Type: '" << _moduleType << "'  CXXConstructExpr " << _astNode << "\n";
}
