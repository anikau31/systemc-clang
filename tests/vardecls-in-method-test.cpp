#include "catch.hpp"

#include "clang/AST/ASTImporter.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/Tooling/Tooling.h"
#include "Matchers.h"

using namespace clang::tooling;
using namespace clang::ast_matchers;
//using namespace sc_ast_matchers;

auto makeModuleMatcher( std::string name ) {
  auto match_module_decls = 
    cxxRecordDecl(
        isDerivedFrom(
          hasName("::sc_core::sc_module") 
          ),
        unless(isDerivedFrom(matchesName("sc_event_queue"))),
        hasName( name )
        );      
  return match_module_decls;
}

class FieldMatcher : public MatchFinder::MatchCallback {
  public:

    void registerMatchers( MatchFinder &finder ) {
      auto match_module_decls = 
        cxxRecordDecl(
            //isExpansionInMainFile(),
            isDerivedFrom(
              hasName("::sc_core::sc_module") 
              ),
            unless(isDerivedFrom(matchesName("sc_event_queue")))
            );      

      auto match_in_ports = 
        cxxRecordDecl(
            forEachDescendant(
              declStmt().bind("sc_in")
 //declStmt(hasAncestor(cxxRecordDecl(isDerivedFrom(hasName("::sc_core::sc_module")))), hasAncestor(cxxMethodDecl()), hasDescendant(varDecl(unless(hasType(cxxRecordDecl(hasName("sc_process_handle"))))))).bind("sc_in")
              //fieldDecl( hasType(cxxRecordDecl(hasName("sc_in"))) ).bind("sc_in")
              )
            );

      //clang-format on


      finder.addMatcher( match_in_ports, this );
    }

    virtual void run( const MatchFinder::MatchResult &result ) {
      cout << " Trying to find sc_in. " << endl;
      if ( auto field_decl = const_cast<Decl*>(result.Nodes.getNodeAs<Decl>("sc_in")) ) {
        cout << " Found an Decl: \n";
        field_decl->dump();
      }


      if ( auto field_decl = const_cast<DeclStmt*>(result.Nodes.getNodeAs<DeclStmt>("sc_in")) ) {
        field_decl->dump();
        //std::string name{ field_decl->getIdentifier()->getNameStart() };
        cout << " Found an declstmt: ";
        //input_port_names.push_back( name );
      }
    }

  public:
    std::vector<std::string> input_port_names;
};


class ModuleMatcher: public MatchFinder::MatchCallback {
  public:
    void registerMatchers( MatchFinder &finder ) {
     // found_module_ = nullptr;
      // Match simple_module
      auto match_simple_module = 
        // clang-format off
        cxxRecordDecl(
            isExpansionInMainFile(),
            isDerivedFrom(
              hasName("::sc_core::sc_module") 
              )
            );      
      //clang-format on
      finder.addMatcher( match_simple_module.bind( "sc_module"), this );

    }

    virtual void matchAST(ASTContext &Conect, Decl *myNode ) {
      Visitor.TraverseDecl(myNode);
    }




    virtual void run( const MatchFinder::MatchResult &result ) {
      if ( auto module_decl = (result.Nodes.getNodeAs<CXXRecordDecl>("sc_module")) ) {
        cout << " Found the one module: " << module_decl->getIdentifier()->getNameStart() << endl;
        std::string name{ module_decl->getIdentifier()->getNameStart() };
        module_names.push_back( name );
        //found_module_ = const_cast<CXXRecordDecl*>(module_decl);


        // Find input ports. 
        MatchFinder field_registry{};
        field_matcher.registerMatchers( field_registry );
        field_registry.match( *module_decl, *result.Context );

      }
    }

    //CXXRecordDecl* getFoundModule() const { return found_module_; }

  public: 
    std::vector<std::string> module_names;
    //CXXRecordDecl* found_module_;
    FieldMatcher field_matcher;
};

TEST_CASE( "Subtree matchers", "[subtree-matchers]") {
  /*
     std::string code = R"(
#include "systemc.h"

SC_MODULE( test ){

sc_in_clk clk;
sc_in<int> in1;
sc_in<int> in2;
  // * Template imports seem to fail.  Still have to see.
  sc_inout<double> in_out;
  sc_out<int> out1;
  sc_out<int> out2;
  sc_signal<int> internal_signal;

  int x;

  void entry_function_1() {
  while(true) {
  }
  }
  SC_CTOR( test ) {
  SC_METHOD(entry_function_1);
  sensitive << clk.pos();
  }
  };

  class MyClass {
  int m1;
  int m2;
  };
  )";

*/
  std::string code = R"(
#include "systemc.h"

SC_MODULE( test ){

  sc_in_clk clk;
  sc_in<int> in1;
  sc_in<int> in2;
  sc_inout<double> in_out;
  sc_out<int> out1;
  sc_out<int> out2;
  sc_signal<int> internal_signal;

  int x;

  void entry_function_1() {
    while(true) {
    }
  }
  SC_CTOR( test ) {
    SC_METHOD(entry_function_1);
    sensitive << clk.pos();
  }
};

SC_MODULE( simple_module ){

  sc_in_clk clk;
  sc_in<int> one;
  sc_in<int> two;
  sc_out<int> out_one;
  int yx;

  void entry_function_1() {
    int x_var;
    double y_var;
    sc_int<4> z_var;
    while(true) {
    }
  }

  SC_CTOR( simple_module ) {
    SC_METHOD(entry_function_1);
    sensitive << clk.pos();
  }
};


int sc_main(int argc, char *argv[]) {
  sc_signal<int> sig1;
  sc_signal<double> double_sig;
  test test_instance("testing");
  test_instance.in1(sig1);
  test_instance.in_out(double_sig);
  test_instance.out1(sig1);

  simple_module simple("simple_second_module");
  return 0;
}
     )";

  std::vector<std::string> args{
    // TODO: Generalize these includes.
    // Perhaps there is a way for cmake to generate these.
    "-D__STD_CONSTANT_MACROS", 
      "-D__STDC_LIMIT_MACROS", 
      "-I/home/twiga/bin/clang-9.0.0/include",
      "-I/home/twiga/bin/clang-9.0.0/lib/clang/9.0.0/include", 
      "-I/home/twiga/code/systemc-2.3.3/systemc/include/", 
      "-I/home/twiga/bin/clang-9.0.0/include", 
      "-std=c++14"
  };

std::unique_ptr<ASTUnit> from_ast =  tooling::buildASTFromCodeWithArgs( code, args );

// Run all the matchers
ModuleMatcher  module_declaration_handler{}; 
MatchFinder matchRegistry{};
module_declaration_handler.registerMatchers( matchRegistry );

matchRegistry.matchAST( from_ast->getASTContext() );

SECTION( "Found sc_modules.") {

  // Two sc_modules should be found.
  REQUIRE( module_declaration_handler.module_names.size() == 2 );
}

}
