#include "FindConstructor.h"
using namespace scpar;

FindConstructor::FindConstructor(CXXRecordDecl * d, llvm::raw_ostream & os):_os(os),
_d(d),
_constructorStmt(nullptr),
pass(1) {

  TraverseDecl(_d);
  pass = 2;
  TraverseStmt(_constructorStmt);
}

FindConstructor::~FindConstructor() {
  _d = nullptr;
  _constructorStmt = nullptr;
}

bool FindConstructor::VisitCXXMethodDecl(CXXMethodDecl * md) {

  switch (pass) {
    case 1:
      {
        if (CXXConstructorDecl * cd = dyn_cast < CXXConstructorDecl > (md)) {
          const FunctionDecl *fd = NULL;

          cd->getBody(fd);

          if (cd->hasBody()) {
            _constructorStmt = cd->getBody();
          }
        }
        break;
      }
    case 2:
      {
        //    _os << "\n\nPass 2 of VisitCXXMethodDecl\n\n";
        break;
      }
    case 3:
      {
        break;
      }
  }
  return true;
}

Stmt *FindConstructor::returnConstructorStmt() {
  return _constructorStmt;
}

void FindConstructor::dump() {
  _os << "\n Module constructor statement dump ";
  _constructorStmt->dump();
}
