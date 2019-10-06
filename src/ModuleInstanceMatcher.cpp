#include "ModuleInstanceMatcher.h"
#include "clang/AST/DeclCXX.h"
#include <iostream>

using namespace std;

void ModuleInstanceMatcher::run( const MatchFinder::MatchResult &result ) {
  cout << "\n ########   ModuleInstanceMatcher ASTMatcher: called  ######### " <<  endl;

  if (const CXXRecordDecl *d = result.Nodes.getNodeAs<clang::CXXRecordDecl>("sc_module")) {
    cout << " Found sc_module: " << d->getIdentifier()->getNameStart() << endl;
  }

  if (const FieldDecl* member_variable = result.Nodes.getNodeAs<clang::FieldDecl>("fieldDecl")){
    cout << "Member variable: " << endl;

    QualType q{member_variable->getType()};
    string fname;

    if (IdentifierInfo *info = member_variable->getIdentifier()) {
      fname = info->getNameStart();
      cout << "\n+ Name: " << info->getNameStart();
    }
  }

  if (const IfStmt *FS = result.Nodes.getNodeAs<clang::IfStmt>("ifStmt")) {
    FS->dump();
  }

  if (const ForStmt *FORS = result.Nodes.getNodeAs<clang::ForStmt>("forStmt")) {
    cout << " ==== > FOR < ===== " << endl;
    FORS->dump();
  }

}

