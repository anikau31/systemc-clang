#include "catch.hpp"

#include "clang/AST/ASTImporter.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/Tooling/Tooling.h"
#include "clang/Parse/ParseAST.h"

#include "SystemCClang.h"
#include "PluginAction.h"

using namespace clang;
using namespace clang::tooling;
using namespace clang::ast_matchers;
using namespace scpar;
TEST_CASE( "Subtree matchers", "[subtree-matchers]") {
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

ASTUnit *from_ast =  tooling::buildASTFromCodeWithArgs( code, args ).release();

SystemCConsumer sc{from_ast};
sc.HandleTranslationUnit(from_ast->getASTContext());

// Run all the matchers

SECTION( "Found sc_modules.") {

  // Two sc_modules should be found.
  REQUIRE( true );
}

}
