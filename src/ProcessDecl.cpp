#include "ProcessDecl.h"
//#include "ReflectionContainerClass.h"
using namespace scpar;

ProcessDecl::ProcessDecl(string t, string n, CXXMethodDecl* entryMethodDecl, Stmt* constructorStmt, EntryFunctionContainer* e)
: _type(t)
, _entryName(n)
, _entryMethodDecl(entryMethodDecl)
, _constructorStmt(constructorStmt)
, _ef(e) {

}


ProcessDecl::~ProcessDecl() {
  // The following points do NOT need to be deleted:_entryMethodDecl, _constructorStmt.
  // This is because they are pointers to the clang AST, which are going to be freed by clang itself.
}

ProcessDecl::ProcessDecl( const ProcessDecl& from) {
  _type = from._type;
  _entryName = from._entryName;
  _entryMethodDecl = from._entryMethodDecl;
  _constructorStmt = from._constructorStmt;
  _ef = from._ef;
}

string ProcessDecl::getType() {
  return _type;
}

string ProcessDecl::getName() {
  return _entryName;
}

CXXMethodDecl* ProcessDecl::getEntryMethodDecl() {
  return _entryMethodDecl;
}

Stmt* ProcessDecl::getConstructorStmt() {
  return _constructorStmt;
}

void ProcessDecl::dump(raw_ostream& os, int tabn) {
  for (int i=0; i < tabn; i++) {
    os << " ";
  }
  os << "ProcessDecl " << this << " '" << _entryName << "' " << _entryMethodDecl << " " << _type << " Constructor " << _constructorStmt;
  _ef->dump(os, ++tabn);

}
