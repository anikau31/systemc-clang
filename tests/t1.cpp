#include "catch.hpp"

#include "clang/AST/ASTImporter.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/Parse/ParseAST.h"
#include "clang/Tooling/Tooling.h"

#include "PluginAction.h"
#include "SystemCClang.h"

// This is automatically generated from cmake.
#include <iostream>
#include "ClangArgs.h"

using namespace clang;
using namespace clang::tooling;
using namespace clang::ast_matchers;
using namespace scpar;

TEST_CASE("Basic parsing checks", "[parsing]") {
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

  simple_module simple("simple_module");
  simple.one(sig1);

  return 0;
}
     )";

  ASTUnit *from_ast =
      tooling::buildASTFromCodeWithArgs(code, systemc_clang::catch_test_args)
          .release();

  SystemCConsumer sc{from_ast};
  sc.HandleTranslationUnit(from_ast->getASTContext());

  auto model{sc.getSystemCModel()};
  // model->dump(llvm::outs());
  auto module_decl{model->getModuleDecl()};

  SECTION("Found sc_modules", "[modules]") {
    // There should be 2 modules identified.
    REQUIRE(module_decl.size() == 2);

    // Check their names, and that their pointers are not nullptr.
    REQUIRE(module_decl["test"] != nullptr);
    REQUIRE(module_decl["simple_module"] != nullptr);
  }

  SECTION("Checking member ports for test instance", "[ports]") {
    // These checks should be performed on the declarations.

    // The module instances have all the information.
    // This is necessary until the parsing code is restructured.
    // There is only one module instance
    auto module_instances{model->getModuleInstanceMap()};
    auto p_module{module_decl["test"]};
    auto test_module{module_instances[p_module].front()};

    // Check if the proper number of ports are found.
    INFO(
        "FAILING: Test will fail because instances and declarations are not "
        "handled the same way.")
    REQUIRE(test_module->getIPorts().size() == 3);
    REQUIRE(test_module->getOPorts().size() == 2);
    REQUIRE(test_module->getIOPorts().size() == 1);
    REQUIRE(test_module->getSignals().size() == 1);
    REQUIRE(test_module->getOtherVars().size() == 1);
    REQUIRE(test_module->getInputStreamPorts().size() == 0);
    REQUIRE(test_module->getOutputStreamPorts().size() == 0);
  }

  SECTION("Checking member ports for test in declarations", "[ports]") {
    // These checks should be performed on the declarations.

    // The module instances have all the information.
    auto test_module{module_decl["test"]};

    // Check if the proper number of ports are found.
    REQUIRE(test_module->getIPorts().size() == 3);
    REQUIRE(test_module->getOPorts().size() == 2);
    REQUIRE(test_module->getIOPorts().size() == 1);
    REQUIRE(test_module->getSignals().size() == 1);
    REQUIRE(test_module->getOtherVars().size() == 1);
    REQUIRE(test_module->getInputStreamPorts().size() == 0);
    REQUIRE(test_module->getOutputStreamPorts().size() == 0);
  }

  SECTION("Checking member ports for simple module instance", "[ports]") {
    auto module_instances{model->getModuleInstanceMap()};
    auto p_module{module_decl["test"]};
    auto test_module{module_instances[p_module].front()};

    // Check if the proper number of ports are found.
    INFO(
        "FAILING: Test will fail because instances and declarations are not "
        "handled the same way.")
    REQUIRE(test_module->getIPorts().size() == 3);
    REQUIRE(test_module->getOPorts().size() == 1);
    REQUIRE(test_module->getIOPorts().size() == 0);
    REQUIRE(test_module->getSignals().size() == 0);
    REQUIRE(test_module->getOtherVars().size() == 1);
    REQUIRE(test_module->getInputStreamPorts().size() == 0);
    REQUIRE(test_module->getOutputStreamPorts().size() == 0);
  }

  SECTION("Checking member ports for simple module ", "[ports]") {
    auto test_module{module_decl["simple_module"]};

    // Check if the proper number of ports are found.
    REQUIRE(test_module->getIPorts().size() == 3);
    REQUIRE(test_module->getOPorts().size() == 1);
    REQUIRE(test_module->getIOPorts().size() == 0);
    REQUIRE(test_module->getSignals().size() == 0);
    REQUIRE(test_module->getOtherVars().size() == 1);
    REQUIRE(test_module->getInputStreamPorts().size() == 0);
    REQUIRE(test_module->getOutputStreamPorts().size() == 0);
  }
}
