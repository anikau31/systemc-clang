#include "Matchers.h"
#include "clang/AST/DeclCXX.h"
#include <iostream>

using namespace std;
using namespace sc_ast_matchers;

//
// Helper functions that can be made private to this class.
//

void printTemplateArguments( ModuleDeclarationMatcher::PortType &found_ports ) {
// Input ports
  for ( const auto &i: found_ports ) {
    llvm::outs() << "name: " << get<0>(i) << ", FieldDecl*: " << get<1>(i)->getFieldDecl();
    get<1>(i)->getTemplateType()->printTemplateArguments(llvm::outs());
    llvm::outs() << "\n";
  }
}

auto parseTemplateType( const FieldDecl *fd ) { 
  //}, const ModuleDeclarationMatcher::PortType &found_ports ) {
  QualType qual_type { fd->getType() };
  const Type *type_ptr { qual_type.getTypePtr() };
  auto template_ptr { new FindTemplateTypes() }; 
  template_ptr->Enumerate( type_ptr );
  return template_ptr;
}


template <typename NodeType>
auto checkMatch(const std::string &name, const MatchFinder::MatchResult &result ){
  return result.Nodes.getNodeAs<NodeType>(name);
}

auto makeFieldMatcher(const std::string &name ) { 

// clang-format off
  return  cxxRecordDecl(
      isExpansionInMainFile(),
      isDerivedFrom(
        hasName("::sc_core::sc_module")
        ),
      forEach(
        fieldDecl(hasType(cxxRecordDecl(hasName(name)))).bind(name)
        )
      );
// clang-format on
}

// End of helper functions

// Register the matchers 
void ModuleDeclarationMatcher::registerMatchers(MatchFinder &finder ) {
// clang-format off
//
auto match_module_decls = 
  cxxRecordDecl(
      isExpansionInMainFile(),
      isDerivedFrom(
        hasName("::sc_core::sc_module") 
        ),
      unless(isDerivedFrom(matchesName("sc_event_queue")))
      );      
  //clang-format on

  // clang-format off
auto match_in_ports = 
  cxxRecordDecl(
      match_module_decls,
      forEach(
          fieldDecl( hasType(cxxRecordDecl(hasName("sc_in"))) ).bind("sc_in")
          )
      );
 
auto match_non_sc_types = cxxRecordDecl(
    match_module_decls,
    forEach(
      fieldDecl(
        unless(hasType(
            cxxRecordDecl(matchesName("sc*"))
            )
          )
        ).bind("other_fields")
      )
    );

auto match_all_ports = 
  cxxRecordDecl(
      match_module_decls,
      anyOf(
        forEach(
          fieldDecl( hasType(cxxRecordDecl(hasName("sc_in"))) ).bind("sc_in")
          ), 
        forEach( 
          fieldDecl( hasType(cxxRecordDecl(hasName("sc_out"))) ).bind("sc_out")
          )
        )
      )


  ;
 

// clang-format off

auto match_clock_ports = makeFieldMatcher("sc_in_clk");
auto match_out_ports = makeFieldMatcher("sc_out");
auto match_in_out_ports = makeFieldMatcher("sc_inout");
auto match_internal_signal = makeFieldMatcher("sc_signal");

// add all the matchers.
//finder.addMatcher( match_module_decls.bind( "sc_module"), this );
finder.addMatcher( match_all_ports.bind( "sc_module"), this );
/*finder.addMatcher( match_clock_ports, this );
finder.addMatcher( match_in_ports, this );
finder.addMatcher( match_out_ports, this );
finder.addMatcher( match_in_out_ports, this );
finder.addMatcher( match_internal_signal, this );
finder.addMatcher( match_non_sc_types, this );
*/
}

void ModuleDeclarationMatcher::run( const MatchFinder::MatchResult &result ) {

  if ( auto decl = const_cast<CXXRecordDecl*>(result.Nodes.getNodeAs<CXXRecordDecl>("sc_module")) ) {
    cout << " Found sc_module: " << decl->getIdentifier()->getNameStart() << endl;
    std::string name{ decl->getIdentifier()->getNameStart() };
    found_declarations_.push_back( std::make_tuple(name, decl) );
  }

  if ( auto fd = result.Nodes.getNodeAs<FieldDecl>("sc_in_clk") ) {
    std::string port_name { fd->getIdentifier()->getNameStart() };
    cout <<" Found sc_in_clk: " << port_name << endl;
    clock_ports_.push_back( std::make_tuple(port_name, 
          new PortDecl(port_name, fd, parseTemplateType(fd)) 
          )
        );
  }

  if ( auto fd = result.Nodes.getNodeAs<FieldDecl>("sc_in") ) {
    auto port_name { fd->getIdentifier()->getNameStart() };
    cout <<" Found sc_in: " << port_name << endl;
    //cout << fd->getParent()->getIdentifier()->getNameStart() << endl;

    in_ports_.push_back( std::make_tuple(port_name, new PortDecl( port_name, fd, parseTemplateType(fd)) ));
  }

  if ( auto fd = checkMatch<FieldDecl>("sc_out", result) ) {
    auto port_name { fd->getIdentifier()->getNameStart() };
    cout <<" Found sc_out: " << port_name << endl;
    out_ports_.push_back( std::make_tuple(port_name, new PortDecl( port_name, fd, parseTemplateType(fd) ) ));
  }

  if ( auto fd = checkMatch<FieldDecl>("sc_inout", result) ) {
    auto port_name { fd->getIdentifier()->getNameStart() };
    cout <<" Found sc_inout: " << port_name << endl;
    inout_ports_.push_back( std::make_tuple(port_name, new PortDecl( port_name, fd, parseTemplateType(fd)) ));
  }
  
  /*
  if ( auto fd = checkMatch<FieldDecl>("sc_signal", result) ) {
    auto signal_name { fd->getIdentifier()->getNameStart() };
    cout <<" Found sc_signal: " << signal_name << endl;
    signal_fields_.push_back( std::make_tuple(signal_name, fd, parseTemplateType(fd) ));
  }

  if ( auto fd = checkMatch<FieldDecl>("other_fields", result) ) {
    auto field_name { fd->getIdentifier()->getNameStart() };
    cout <<" Found others fields: " << field_name << endl;
    other_fields_.push_back( std::make_tuple(field_name, fd, parseTemplateType(fd) ));
  }

  if (auto spec_decl = result.Nodes.getNodeAs<ClassTemplateSpecializationDecl*>("sp_dcl_bd_name_")) {
    cout <<" Found template : \n";

  }
  */
}

const ModuleDeclarationMatcher::ModuleDeclarationTuple& ModuleDeclarationMatcher::getFoundModuleDeclarations() const { return found_declarations_; }

const ModuleDeclarationMatcher::PortType& ModuleDeclarationMatcher::getFields(const std::string & port_type ) {
  if (port_type == "sc_in") { return in_ports_; }
  if (port_type == "sc_inout") { return inout_ports_; }
  if (port_type == "sc_out") { return out_ports_; }
  if (port_type == "sc_signal") { return signal_fields_; }
  if (port_type == "sc_in_clk") { return clock_ports_; }
  if (port_type == "others") { return other_fields_; }
  // You should never get here.
  assert(true);
}

void ModuleDeclarationMatcher::dump() {
  for ( const auto &i: found_declarations_ ) {
    cout << "module name: " << get<0>(i) << ", " << get<1>(i) << std::endl;
  }

  // Map structure
  // Input ports
  // for ( const auto &i: in_ports_ ) {
    // llvm::outs() << "name: " << i.first << " ";
    // (i.second)->printTemplateArguments(llvm::outs());
    // llvm::outs() << "\n";
  // }
//

  printTemplateArguments( clock_ports_ );
  printTemplateArguments( in_ports_ );
  printTemplateArguments( out_ports_ );
  printTemplateArguments( inout_ports_ );
  printTemplateArguments( signal_fields_ );
  printTemplateArguments( other_fields_ );
}


