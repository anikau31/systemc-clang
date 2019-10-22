#include "Matchers.h"
#include "clang/AST/DeclCXX.h"
#include <iostream>

using namespace std;
using namespace sc_ast_matchers;

template <typename NodeType>
auto checkMatch(const std::string &name, const MatchFinder::MatchResult &result ){
  return result.Nodes.getNodeAs<NodeType>(name);
}

auto makeFieldMatcher(const std::string &name ) { 
  return  cxxRecordDecl(
      isExpansionInMainFile(),
      isDerivedFrom(
        hasName("::sc_core::sc_module")
        ),
      forEach(
        fieldDecl(hasType(cxxRecordDecl(hasName(name)))).bind(name)
        )
      );
}


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

/*
auto matchPorts = 
  cxxRecordDecl(
      isExpansionInMainFile(),
      isDerivedFrom(
        hasName("::sc_core::sc_module")
        ),
      findAll(
          fieldDecl(hasType(cxxRecordDecl(matchesName("sc_*")))).bind("sc_ports")
        )
      );
      */

auto match_in_ports = 
  cxxRecordDecl(
      isExpansionInMainFile(),
      isDerivedFrom(
        hasName("::sc_core::sc_module")
        ),
      forEach(
          fieldDecl( hasType(cxxRecordDecl(hasName("sc_in"))) ).bind("sc_in")
          )
      );
 
auto match_non_sc_types = cxxRecordDecl(
    isExpansionInMainFile(), 
    isDerivedFrom(hasName("::sc_core::sc_module")), 
    unless(isDerivedFrom(matchesName("sc_event_queue"))),  forEach(fieldDecl(unless(hasType(cxxRecordDecl(matchesName("sc*"))))).bind("other_fields")));

auto match_out_ports = makeFieldMatcher("sc_out");
auto match_in_out_ports = makeFieldMatcher("sc_inout");
auto match_internal_signal = makeFieldMatcher("sc_signal");
auto matchClock = makeFieldMatcher("sc_clock");

// add all the matchers.
finder.addMatcher( matchModuleDeclarations.bind( "sc_module"), this );
finder.addMatcher( match_in_ports, this );
finder.addMatcher( match_out_ports, this );
finder.addMatcher( match_in_out_ports, this );
finder.addMatcher( match_internal_signal, this );
finder.addMatcher( matchClock, this );
finder.addMatcher( match_non_sc_types, this );
}

void ModuleDeclarationMatcher::run( const MatchFinder::MatchResult &result ) {

  if ( auto decl = const_cast<CXXRecordDecl*>(result.Nodes.getNodeAs<CXXRecordDecl>("sc_module")) ) {
    cout << " Found sc_module: " << decl->getIdentifier()->getNameStart() << endl;
    std::string name{ decl->getIdentifier()->getNameStart() };
    found_declarations_.push_back( std::make_tuple(name, decl) );
  }

  if ( auto fd = result.Nodes.getNodeAs<FieldDecl>("sc_in") ) {
    cout <<" Found sc_in: " << fd->getIdentifier()->getNameStart() << endl;
  }

  if ( auto fd = checkMatch<FieldDecl>("sc_out", result) ) {
    cout <<" Found sc_out: " << fd->getIdentifier()->getNameStart() << endl;
  }

  if ( auto fd = checkMatch<FieldDecl>("sc_inout", result) ) {
    cout <<" Found sc_inout: " << fd->getIdentifier()->getNameStart() << endl;
  }
  
  if ( auto signal = checkMatch<FieldDecl>("sc_signal", result) ) {
    cout <<" Found sc_signal: " << signal->getIdentifier()->getNameStart() << endl;
  }

  if ( auto otherFields = checkMatch<FieldDecl>("other_fields", result) ) {
    cout <<" Found others: " << otherFields->getIdentifier()->getNameStart() << endl;
  }

  if (auto spec_decl = result.Nodes.getNodeAs<ClassTemplateSpecializationDecl*>("sp_dcl_bd_name_")) {
    cout <<" Found template : \n";

  }
}

const ModuleDeclarationMatcher::ModuleDeclarationTuple& ModuleDeclarationMatcher::getFoundModuleDeclarations() const { return found_declarations_; }

void ModuleDeclarationMatcher::dump() {
  for ( const auto& i : found_declarations_ ) {
    cout << "module name: " << get<0>(i) << ", " << get<1>(i) << std::endl;
  }
}

