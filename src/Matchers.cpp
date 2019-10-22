#include "Matchers.h"
#include "clang/AST/DeclCXX.h"
#include <iostream>

using namespace std;
using namespace sc_ast_matchers;

// Register the matchers 
void ModuleDeclarationMatcher::registerMatchers(MatchFinder &finder ) {
// clang-format off
//
// Match sc_ports
//
// Match sc_modules
//
auto matchModuleDeclarations = 
  cxxRecordDecl(
      isExpansionInMainFile(),
      isDerivedFrom(
        hasName("::sc_core::sc_module") 
        )
      );      
  //clang-format on

finder.addMatcher( matchModuleDeclarations.bind( "sc_module"), this );

}

void ModuleDeclarationMatcher::run( const MatchFinder::MatchResult &result ) {

  if ( CXXRecordDecl* decl = const_cast<CXXRecordDecl*>(result.Nodes.getNodeAs<clang::CXXRecordDecl>("sc_module")) ) {
    cout << " Found sc_module: " << decl->getIdentifier()->getNameStart() << endl;
    //decl->dump();
    std::string name{ decl->getIdentifier()->getNameStart() };
    found_declarations_.push_back( std::make_tuple(name, decl) );
  }
}

const ModuleDeclarationMatcher::ModuleDeclarationTuple& ModuleDeclarationMatcher::getFoundModuleDeclarations() const { return found_declarations_; }

void ModuleDeclarationMatcher::dump() {
  for ( const auto& i : found_declarations_ ) {
    cout << "module name: " << get<0>(i) << ", " << get<1>(i) << std::endl;
  }
}

