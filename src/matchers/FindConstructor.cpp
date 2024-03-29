#include "FindConstructor.h"

using namespace systemc_clang;

FindConstructor::FindConstructor(const clang::CXXRecordDecl *declaration,
                                 llvm::raw_ostream &os)
    : os_{os},
      declaration_{declaration},
      constructor_stmt_{nullptr},
      constructor_decl_{nullptr},
      pass_{1} {
  TraverseDecl(const_cast<clang::CXXRecordDecl *>(declaration_));
  pass_ = 2;
  TraverseStmt(constructor_stmt_);
}

FindConstructor::~FindConstructor() {
  declaration_ = nullptr;
  constructor_stmt_ = nullptr;
}

bool FindConstructor::shouldVisitTemplateInstantiations() const { return true; }

bool FindConstructor::VisitCXXConstructorDecl(
    clang::CXXConstructorDecl *ctor_decl) {
  constructor_decl_ = ctor_decl;

  return true;
}

bool FindConstructor::VisitCXXMethodDecl(
    clang::CXXMethodDecl *method_declaration) {
  switch (pass_) {
    case 1: {
      // constructor_decl_ = dyn_cast<CXXConstructorDecl>(method_declaration);
      // llvm::outs() << "setting the constructor_decl_ to " <<
      // constructor_decl_
      //            << "\n ";
      if (constructor_decl_) {
        const clang::FunctionDecl *fd{nullptr};
        constructor_decl_->getBody(fd);
        if (constructor_decl_->hasBody()) {
          constructor_stmt_ = constructor_decl_->getBody();
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

clang::CXXConstructorDecl *FindConstructor::getConstructorDecl() const {
  return constructor_decl_;
}

const clang::CXXRecordDecl *FindConstructor::getAsCXXRecordDecl() const {
  return declaration_;
}

clang::Stmt *FindConstructor::getConstructorStmt() const {
  return constructor_stmt_;
}

void FindConstructor::dump() const { constructor_stmt_->dump(); }
