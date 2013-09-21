#include <string>
#include "ClassMemberFunctions.h"
#include "FindTemplateTypes.h"

using namespace scpar;
using namespace std;

ClassMemberFunctions::~ClassMemberFunctions() {

  // _astNode does *NOT* need to be deleted because clang should
  // be responsible for freeing the memory.
}

ClassMemberFunctions::ClassMemberFunctions():
  _name("NONE"),_astNode(NULL) {

}

ClassMemberFunctions::ClassMemberFunctions(const string& name, CXXMethodDecl* fd) :
  _name(name),_astNode(fd) {

}

ClassMemberFunctions::ClassMemberFunctions(const ClassMemberFunctions& from) {
  _name = from._name;
  _astNode = from._astNode;
}

void ClassMemberFunctions::ptrCheck() {
  assert(!(_astNode == NULL));
}

string ClassMemberFunctions::getName() {
  return _name;
}

CXXMethodDecl* ClassMemberFunctions::getASTNode() {
  ptrCheck();
  return _astNode;
}

void ClassMemberFunctions::dump(raw_ostream& os, int tabn) {
  ptrCheck();
  Utility u; u.tabit(os, tabn);
  //for (int i=0; i < tabn; i++) {
//    os << " ";
//  }
  os << "ClassMemberFunctions " << this << " '" << _name << "' CXXMethodecl* " << getASTNode();
}
