#include "catch.hpp"

#include "clang/AST/ASTImporter.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/Parse/ParseAST.h"
#include "clang/Tooling/Tooling.h"

#include "PluginAction.h"
#include "SystemCClang.h"

// This is automatically generated from cmake.
#include "ClangArgs.h"

using namespace clang;
using namespace clang::tooling;
using namespace clang::ast_matchers;
using namespace scpar;

TEST_CASE("Failing: unbounded signals in modules identified",
          "[failing-tests]") {
  std::string code = R"(
#include "systemc.h"

SC_MODULE( test ){

  // input ports
  sc_in_clk clk;
  sc_in<int> in1;
  sc_in<int> in2;
  // inout ports
  sc_inout<double> in_out;
  // output ports
  sc_out<int> out1;
  sc_out<int> out2;
  //signals
  sc_signal<int> internal_signal;

  // others
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

int sc_main(int argc, char *argv[]) {
  test test_instance("testing");

  return 0;
}
     )";

  ASTUnit *from_ast =
      tooling::buildASTFromCodeWithArgs(code, systemc_clang::catch_test_args)
          .release();

  SystemCConsumer sc{from_ast};
  sc.HandleTranslationUnit(from_ast->getASTContext());
  auto model{sc.getSystemCModel()};
  auto module_decl{model->getModuleDecl()};

  SECTION("No ports bound for test declaration", "[ports]") {
    // The module instances have all the information.
    auto test_module{module_decl["test"]};
    // There is only one module instance

    // Check if the proper number of ports are found.
    INFO("FAIL_TEST: A module must have a port bound for it to be recognized.");
    REQUIRE(test_module != nullptr);
  }
}
