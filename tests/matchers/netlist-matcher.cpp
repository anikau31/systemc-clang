/* clang-format off */
// This must be the order of includes.
#include "catch.hpp"
#include "SystemCClang.h"
#include "Matchers.h"
/* clang-format on */

// This is automatically generated from cmake.
#include "ClangArgs.h"
#include "Testing.h"

using namespace systemc_clang;
using namespace sc_ast_matchers;

#undef DEBUG_TYPE
#define DEBUG_TYPE "Tests"

TEST_CASE("Testing top-level module: test", "[top-module]") {
  /// Enable debug
  llvm::DebugFlag = true;
  llvm::setCurrentDebugType("NetlistMatcher");

  std::string code{systemc_clang::read_systemc_file(
      systemc_clang::test_data_dir, "netlist-matcher-input.cpp")};

  ASTUnit *from_ast =
      tooling::buildASTFromCodeWithArgs(code, systemc_clang::catch_test_args)
          .release();

  // Name of top-level module declaration.
  // std::string top{"test"};

  SystemCConsumer sc{from_ast};
  sc.HandleTranslationUnit(from_ast->getASTContext());
  auto model{sc.getSystemCModel()};
  auto instances{model->getInstances()};

  // There are three module declarations found: test, dut, sub_module
  REQUIRE(instances.size() == 3);

  auto found_module_testing{model->getInstance("testing")};
  REQUIRE(found_module_testing != nullptr);

  auto found_module_submodule{model->getInstance("SUBMODULE")};
  REQUIRE(found_module_submodule != nullptr);

  SECTION("Testing top-level module: test", "[top module]") {
    // There should be two modules because there are two instances.
    INFO("Top-level module specified as test.");

    auto found_decl{found_module_testing};
    REQUIRE(found_decl->getIPorts().size() == 3);
    REQUIRE(found_decl->getOPorts().size() == 2);
    // This is 4 because sc_buffer is also inheriting from the signal interface.
    REQUIRE(found_decl->getSignals().size() == 0);
    // 1 non-array, and 2 array others
    REQUIRE(found_decl->getOtherVars().size() == 0);
    REQUIRE(found_decl->getNestedModules().size() == 1);

    // TODO: Check the template parameters.
    //

    // test_module_inst
    auto port_bindings{found_decl->getPortBindings()};
    std::vector<std::string> test_ports{"clk", "inS", "outS"};

    LLVM_DEBUG(llvm::dbgs() << "Number of port bindings: " << port_bindings.size() << "\n";);
    // Print all the port binding information
    for (auto const &p : port_bindings) {
      auto pb{p.second};
      LLVM_DEBUG(pb->dump(););
    }

    for (auto const &pname : test_ports) {
      auto found_it{port_bindings.find(pname)};
      // Actually found the name
      REQUIRE(found_it != port_bindings.end());
      // Check now if the names
      std::string port_name{found_it->first};
      PortBinding *binding{found_it->second};
      std::string as_string{binding->toString()};

      if (port_name == "clk") {
        REQUIRE(as_string == "test test_instance testing clk");
      }
      if (port_name == "inS") {
        REQUIRE(as_string == "test test_instance testing sig1");
      }
      if (port_name == "outS") {
        REQUIRE(as_string == "test test_instance testing sig1");
      }
    }
  }

  SECTION("Testing submodule", "[submodule]") {
    auto found_decl{found_module_submodule};
    REQUIRE(found_decl->getIPorts().size() == 1);
    REQUIRE(found_decl->getOPorts().size() == 1);
    REQUIRE(found_decl->getSignals().size() == 0);
    REQUIRE(found_decl->getOtherVars().size() == 0);

    auto port_bindings{found_decl->getPortBindings()};
    std::vector<std::string> test_ports{"input", "output"};

    for (auto const &pname : test_ports) {
      auto found_it{port_bindings.find(pname)};
      // Actually found the name
      REQUIRE(found_it != port_bindings.end());
      // Check now if the names
      std::string port_name{found_it->first};
      PortBinding *binding{found_it->second};
      std::string as_string{binding->toString()};

      if (port_name == "input") {
        REQUIRE(as_string == "submodule sub_module_member SUBMODULE inS");
      }

      if (port_name == "output") {
        REQUIRE(as_string == "submodule sub_module_member SUBMODULE outS");
      }
    }
  }
}
