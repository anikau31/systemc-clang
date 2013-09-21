#include <string>
#include "ClassDataMembers.h"
#include "FindTemplateTypes.h"

using namespace scpar;
using namespace std;

ClassDataMembers::~ClassDataMembers() {

  // _astNode does *NOT* need to be deleted because clang should
  // be responsible for freeing the memory.
}

ClassDataMembers::ClassDataMembers():
  _name("NONE"),_astNode(NULL) {

}

ClassDataMembers::ClassDataMembers(const string& name, double size, FieldDecl* fd) :
  _name(name),_size(size),_astNode(fd) {

}

ClassDataMembers::ClassDataMembers(const ClassDataMembers& from) {
  _name = from._name;
  _astNode = from._astNode;
  _size = from._size;
}

void ClassDataMembers::ptrCheck() {
  assert(!(_astNode == NULL));
}

FieldDecl* ClassDataMembers::getASTNode() {
  ptrCheck();
  return _astNode;
}
double ClassDataMembers::getSize() {
  return _size;
}
void ClassDataMembers::dump(raw_ostream& os, int tabn) {
  ptrCheck();

    for (int i=0; i < tabn; i++) {
    os << " ";
  }  

  os << "ClassDataMembers " << this << " '" << _name << "' FieldDecl " << getASTNode()<<" Size: "<<_size;;
    
}
