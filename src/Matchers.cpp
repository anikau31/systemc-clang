#include "Matchers.h"
#include "clang/AST/DeclCXX.h"
#include <iostream>

using namespace std;

void ModuleDeclarationMatcher::run( const MatchFinder::MatchResult& result ) {
  cout << "\n ########   ModuleInstanceMatcher ASTMatcher: called  ######### " <<  endl;

  if ( const CXXRecordDecl* decl = result.Nodes.getNodeAs<clang::CXXRecordDecl>("sc_module") ) {
    cout << " Found sc_module: " << decl->getIdentifier()->getNameStart() << endl;
    std::string name{ decl->getIdentifier()->getNameStart() };
    foundDeclarations_.push_back( std::make_tuple(name, decl) );
  }
}

const ModuleDeclarationMatcher::ModuleDeclarationTuple& ModuleDeclarationMatcher::getFoundModuleDeclarations() const { return foundDeclarations_; }

void ModuleDeclarationMatcher::dump() {
  for ( const auto& i : foundDeclarations_ ) {
    cout << "module name: " << get<0>(i) << ", " << get<1>(i) << std::endl;
  }
}

