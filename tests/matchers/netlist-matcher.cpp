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
  // llvm::setCurrentDebugType("NetlistMatcher");

  std::string code{systemc_clang::read_systemc_file(
      systemc_clang::test_data_dir, "netlist-matcher-input.cpp")};

  ASTUnit *from_ast =
      tooling::buildASTFromCodeWithArgs(code, systemc_clang::catch_test_args)
          .release();

  // Name of top-level module declaration.

  SystemCConsumer sc{from_ast};
  sc.HandleTranslationUnit(from_ast->getASTContext());
  auto model{sc.getSystemCModel()};
  auto instances{model->getInstances()};

  // There are five module instances found:
  // test, dut, sub_module_member submod_1d, submod_2d, submod_3d
  REQUIRE(instances.size() == 6);

  /// testing instance
  auto found_module_testing{model->getInstance("testing")};
  REQUIRE(found_module_testing != nullptr);

  auto found_module_submodule{model->getInstance("SUBMODULE")};
  REQUIRE(found_module_submodule != nullptr);

  auto dut_module{model->getInstance("dut")};
  REQUIRE(dut_module != nullptr);

  /// Instance testing.
  SECTION("Test port bindings for instance testing", "[testing]") {
    auto found_decl{found_module_testing};
    REQUIRE(found_decl->getIPorts().size() == 4);
    REQUIRE(found_decl->getOPorts().size() == 2);
    // This is 4 because sc_buffer is also inheriting from the signal interface.
    REQUIRE(found_decl->getSignals().size() == 0);
    // 1 non-array, and 2 array others
    REQUIRE(found_decl->getOtherVars().size() == 0);
    REQUIRE(found_decl->getNestedModuleInstances().size() == 4);

    /// TODO: Check for submodules, and port bindings.
    auto port_bindings{found_decl->getPortBindings()};
    std::string instance_name{"testing"};
    std::vector<std::string> caller_names{"sub_module_member", "submod_1d",
                                          "submodules_2d", "submodules_3d"};

    LLVM_DEBUG(llvm::dbgs() << "Number of port bindings: "
                            << port_bindings.size() << "\n";);

    // Print all the port binding information
    for (auto const &p : port_bindings) {
      auto pb{p.second};
      LLVM_DEBUG(llvm::dbgs() << "bind_name: " << p.first << "\n");
      LLVM_DEBUG(pb->dump(););
    }

    /// Ensure that all checks are encountered.
    int check_count{5};
    for (auto const &binding : port_bindings) {
      PortBinding *pb{binding.second};
      std::string caller_name{pb->getCallerInstanceName()};
      std::string port_name{pb->getCallerPortName()};
      std::string as_string{pb->toString()};

      llvm::outs() << "\n";
      llvm::outs() << "check string: " << as_string << "\n";
      llvm::outs() << "inst name: " << pb->getCallerInstanceName()
                   << ", port name: " << pb->getCallerPortName() << "\n\n";

      if (caller_name == "sub_module_member") {
        if (port_name == "input") {
          REQUIRE(as_string == "submodule sub_module_member SUBMODULE input inS");
          llvm::outs() << "@@@@@ 1st\n";
          --check_count;
        }
        if (port_name == "output") {
          REQUIRE(as_string == "submodule sub_module_member SUBMODULE output outS");
          --check_count;
        }
      }

      if (caller_name == "submod_1d") {
        if (port_name == "output") {
          REQUIRE(as_string == "submodule submod_1d 1d_submod_0 0 output outS");
          --check_count;
        }
      }

      if (caller_name == "submodules_2d") {
        if (port_name == "input") {
          REQUIRE(as_string == "submodule submodules_2d submod_0_0 1 2 input inS");
          --check_count;
        }

        if (port_name == "output") {
          REQUIRE(as_string ==
                  "submodule submodules_2d submod_0_0 0 1 output submodules_2d 1 2 "
                  "output");
          --check_count;
        }

        if (caller_name == "submodules_3d") {
          if (port_name == "input") {
            REQUIRE(as_string ==
                    "submodule submdules_3d submod_0_0_0 1 3 1 input inS");
            --check_count;
          }
        }
        //    }
      }
    }
    REQUIRE(check_count == 0);
  }
  /// Instance: dut.
  SECTION("Test port bindings for instance testing", "[dut]") {
    auto found_decl{dut_module};
    REQUIRE(found_decl->getIPorts().size() == 0);
    REQUIRE(found_decl->getOPorts().size() == 0);
    REQUIRE(found_decl->getSignals().size() == 2);
    REQUIRE(found_decl->getOtherVars().size() == 0);
    REQUIRE(found_decl->getNestedModuleInstances().size() == 1);

    std::vector<std::string> caller_names{"test_instance"};

    // Test the port bindings for the testing instance.
    auto port_bindings{found_decl->getPortBindings()};

    LLVM_DEBUG(llvm::dbgs() << "Number of port bindings: "
                            << port_bindings.size() << "\n";);
    // Print all the port binding information
    for (auto const &p : port_bindings) {
      auto pb{p.second};
      LLVM_DEBUG(pb->dump(););
    }

    int check_count{3};
    for (auto const &binding : port_bindings) {
      PortBinding *pb{binding.second};
      std::string port_name{pb->getCallerPortName()};
      std::string caller_name{pb->getCallerInstanceName()};
      std::string as_string{pb->toString()};
      llvm::outs() << "check string: " << as_string << "\n";
      if (caller_name == "test_instance") {
        if (port_name == "clk") {
          REQUIRE(as_string == "test test_instance testing clk clk");
          --check_count;
        }
        if (port_name == "inS") {
          REQUIRE(as_string == "test test_instance testing inS sig1");
          --check_count;
        }
        if (port_name == "outS") {
          REQUIRE(as_string == "test test_instance testing outS sig1");
          --check_count;
        }
      }
    }
    REQUIRE(check_count == 0);

    SECTION("Testing submodule", "[submodule]") {
      auto found_decl{found_module_submodule};
      REQUIRE(found_decl->getIPorts().size() == 1);
      REQUIRE(found_decl->getOPorts().size() == 1);
      REQUIRE(found_decl->getSignals().size() == 0);
      REQUIRE(found_decl->getOtherVars().size() == 0);

      auto port_bindings{found_decl->getPortBindings()};
      REQUIRE(port_bindings.size() == 0 );
    }
  }
}
