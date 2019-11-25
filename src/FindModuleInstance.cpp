#include "FindModuleInstance.h"
#include "FindTemplateTypes.h"
#include "clang/AST/DeclTemplate.h"
#include "clang/AST/Type.h"
//#include "clang/Basic/SourceManager.h"

using namespace scpar;

string FindModuleInstance::getInstanceName() const { return instance_name_; }

FindModuleInstance::FindModuleInstance(CXXRecordDecl *declaration,
                                       llvm::raw_ostream &os)
    : declaration_{declaration}, os_{os} {
  if (declaration->hasDefinition() == true) {
    TraverseDecl(declaration);
  }
}

bool FindModuleInstance::VisitFieldDecl(FieldDecl *fdecl) {
  // os_ << "#### FOUND FIELD DECL\n";
  QualType q{fdecl->getType()};
  string fname;

  if (IdentifierInfo *info = fdecl->getIdentifier()) {
    fname = info->getNameStart();
    // os_ << "\n+ Name: " << info->getNameStart();
    // os_ << "\n+ Type: " << q.getAsString();
  }

  // fdecl->dump();
  return true;
}

bool FindModuleInstance::VisitCXXConstructExpr(CXXConstructExpr *expr) {
  // os_ << "#### FOUND CONSTRUCTOR \n";
  // expr->dump();
  return true;
}

FindModuleInstance::~FindModuleInstance() { declaration_ = nullptr; }

void FindModuleInstance::dump() {
  os_ << "\n ============== FindModuleInstance ===============";
  os_ << "\n:> module name: " << instance_name_
      << ", CXXRecordDecl*: " << declaration_ << "\n";
  os_ << "\n ============== END FindModuleInstance ===============";
}
