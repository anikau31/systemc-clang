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

  simple_module simple("simple_module_instance");
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

  // This provides the module declarations.
  auto module_decl{model->getModuleDecl()};
  auto module_instance_map{model->getModuleInstanceMap()};

  // Want to find an instance named "testing".

  ModuleDecl *test_module{model->getInstance("testing")};;
  ModuleDecl *simple_module{model->getInstance("simple_module_instance")};

/*
  for (auto const &element : module_instance_map) {
    auto instance_list{element.second};

    auto test_module_it = std::find_if(
        instance_list.begin(), instance_list.end(), [](const auto &instance) {
          return instance->getInstanceName() == "testing";
        });
    test_module = *test_module_it;
    if (test_module_it != instance_list.end()) {
      break;
    }
  }

  for (auto const &element : module_instance_map) {
    auto instance_list{element.second};
    auto simple_module_it = std::find_if(
        instance_list.begin(), instance_list.end(), [](const auto &instance) {
          return instance->getInstanceName() == "simple_module_instance";
        });
    simple_module = *simple_module_it;
    if (simple_module_it != instance_list.end()) {
      break;
    }
  }
  */

  SECTION("Found sc_module instances", "[instances]") {
    // There should be 2 modules identified.
    INFO("Checking number of sc_module declarations found: "
         << module_decl.size());

    REQUIRE(module_decl.size() == 2);

    REQUIRE(test_module != nullptr);
    REQUIRE(simple_module != nullptr);

    INFO("Checking member ports for test instance.");
    // These checks should be performed on the declarations.

    // The module instances have all the information.
    // This is necessary until the parsing code is restructured.
    // There is only one module instance
    // auto module_instances{model->getModuleInstanceMap()};
    // auto p_module{module_decl.find("test")};
    //
    //
    auto test_module_inst{test_module};

    // Check if the proper number of ports are found.
    REQUIRE(test_module_inst->getIPorts().size() == 3);
    REQUIRE(test_module_inst->getOPorts().size() == 2);
    REQUIRE(test_module_inst->getIOPorts().size() == 1);
    REQUIRE(test_module_inst->getSignals().size() == 1);
    REQUIRE(test_module_inst->getOtherVars().size() == 1);
    REQUIRE(test_module_inst->getInputStreamPorts().size() == 0);
    REQUIRE(test_module_inst->getOutputStreamPorts().size() == 0);

    INFO("Checking member ports for simple module instance.");
    auto simple_module_inst{simple_module};

    // Check if the proper number of ports are found.
    REQUIRE(simple_module_inst->getIPorts().size() == 3);
    REQUIRE(simple_module_inst->getOPorts().size() == 1);
    REQUIRE(simple_module_inst->getIOPorts().size() == 0);
    REQUIRE(simple_module_inst->getSignals().size() == 0);
    REQUIRE(simple_module_inst->getOtherVars().size() == 1);
    REQUIRE(simple_module_inst->getInputStreamPorts().size() == 0);
    REQUIRE(simple_module_inst->getOutputStreamPorts().size() == 0);
  }
}
