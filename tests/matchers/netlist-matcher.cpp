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
  // std::string top{"test"};

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
    std::vector<std::string> test_ports{"input", "output"};
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

    for (auto const &binding : port_bindings) {
        for (auto const &pname : test_ports) {
          // std::string bind_name{cname + pname};
          // llvm::dbgs() << "[[[[[[[[[[[[[[[[[[[[[ Binding name: " << bind_name
          //            << "\n";
          // auto found_it{port_bindings.find(bind_name)};
          // Actually found the name
          // REQUIRE(found_it != port_bindings.end());

          // if (found_it != port_bindings.end()) {
          // Check now if the names
          PortBinding *pb{binding.second};
          std::string port_name{pb->getCallerPortName()};
          std::string caller_name{ pb->getCallerInstanceName()};
          std::string as_string{pb->toString()};

          llvm::outs() << "check string: " << as_string << "\n";
          llvm::outs() << "inst name: " << pb->getCallerInstanceName()
                       << ", port name: " << pb->getCallerPortName() << "\n";

          if ( caller_name == "sub_module_member") {
            if (port_name == "input") {
              REQUIRE(as_string == "submodule sub_module_member SUBMODULE inS");
            }
            if (port_name == "output") {
              REQUIRE(as_string ==
                      "submodule sub_module_member SUBMODULE outS");
            }
          }

          if ( caller_name == "submod_1d") {
            if (port_name == "output") {
              REQUIRE(as_string == "submodule submod_1d 1d_submod_0 outS");
            }
          }

          if (caller_name == "submodules_2d") {
            if (port_name == "input") {
              REQUIRE(as_string == "submodule submodules_2d submod_0_0 inS");
              llvm::outs()
                  << " #################@@@@@@@@@@@@@@@@@@@@@@ FOUND \n";
            }

            if (port_name == "output") {
              REQUIRE(as_string == "submodule submodules_2d submod_0_0 submodules_2d output");
              llvm::outs()
                  << " #################@@@@@@@@@@@@@@@@@@@@@@ OUTPUT\n";
            }

            if (caller_name == "submodules_3d") {
              if (port_name == "input") {
                REQUIRE(as_string == "submodule submdules_3d submod_0_0_0 inS");
                llvm::outs()
                    << " #################@@@@@@@@@@@@@@@@@@@@@@ OUTPUT\n";
              }
            }
          }
        }
    }
  }
  /// Instance: dut.
  SECTION("Test port bindings for instance testing", "[dut]") {
    auto found_decl{dut_module};
    REQUIRE(found_decl->getIPorts().size() == 0);
    REQUIRE(found_decl->getOPorts().size() == 0);
    REQUIRE(found_decl->getSignals().size() == 2);
    REQUIRE(found_decl->getOtherVars().size() == 0);
    REQUIRE(found_decl->getNestedModuleInstances().size() == 1);

    // Test the port bindings for the testing instance.
    auto port_bindings{found_decl->getPortBindings()};
    std::string instance_name{"test_instance"};
    std::vector<std::string> test_ports{"clk", "inS", "outS"};

    LLVM_DEBUG(llvm::dbgs() << "Number of port bindings: "
                            << port_bindings.size() << "\n";);
    // Print all the port binding information
    for (auto const &p : port_bindings) {
      auto pb{p.second};
      LLVM_DEBUG(llvm::dbgs()
                 << "bind_name: " << instance_name + p.first << "\n");
      LLVM_DEBUG(pb->dump(););
    }

    for (auto const &pname : test_ports) {
      std::string bind_name{instance_name + pname};
      llvm::dbgs() << "Binding name: " << bind_name << "\n";
      auto found_it{port_bindings.find(bind_name)};
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
    std::string instance_name{"SUBMODULE"};
    std::vector<std::string> test_ports{"input", "output"};

    for (auto const &pname : test_ports) {
      std::string bind_name{instance_name + pname};
      auto found_it{port_bindings.find(bind_name)};
      // There are 0 bindings in the submodule.
      REQUIRE(found_it == port_bindings.end());
      /*
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
      */
    }
  }
}
