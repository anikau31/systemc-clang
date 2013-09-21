#include "SensitivityDecl.h"

using namespace scpar;

SensitivityDecl::SensitivityDecl(string name): _name(name) {
}

SensitivityDecl::SensitivityDecl(const SensitivityDecl& from) {
  _name = from._name;
}

string SensitivityDecl::getName() {
  return _name;
}


