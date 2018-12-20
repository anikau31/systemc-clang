#include "FindConstructor.h"

using namespace scpar;

FindConstructor::FindConstructor( CXXRecordDecl *declaration, llvm::raw_ostream &os ) :
  os_{os},
  declaration_{declaration},
  constructor_stmt_{nullptr},
  pass_{1} {

    TraverseDecl( declaration_ );
    pass_ = 2;
    TraverseStmt( constructor_stmt_ );
  }

FindConstructor::~FindConstructor() {
  declaration_ = nullptr;
  constructor_stmt_ = nullptr;
}

bool FindConstructor::VisitCXXMethodDecl( CXXMethodDecl *method_declaration ) {
  switch ( pass_ ) {
  case 1:  {
    if (CXXConstructorDecl *cd = dyn_cast< CXXConstructorDecl >(method_declaration)) {
      const FunctionDecl *fd{nullptr};
      cd->getBody( fd );
      if ( cd->hasBody() ) {
        constructor_stmt_ = cd->getBody();
      }
    }
    break;
  }
  case 2: {
    //    os_ << "\n\nPass 2 of VisitCXXMethodDecl\n\n";
    break;
  }
  case 3: {
    break;
  }
  }
  return true;
}

Stmt * FindConstructor::returnConstructorStmt() const {
  return constructor_stmt_;
}

void FindConstructor::dump() const {
  os_ << "\n Module constructor statement dump ";
  constructor_stmt_->dump();
}
