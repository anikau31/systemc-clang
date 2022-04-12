/* clang-format off */
// This must be the order of includes.
#include <doctest.h>
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

TEST_CASE("Testing top-level module: test"){
  /// Enable debug
  llvm::DebugFlag = false;

  std::string code{systemc_clang::read_systemc_file(
      systemc_clang::test_data_dir, "netlist-matcher-templated-input.cpp")};

  ASTUnit *from_ast =
      tooling::buildASTFromCodeWithArgs(code, systemc_clang::catch_test_args)
          .release();

  SystemCConsumer sc{from_ast};
  sc.HandleTranslationUnit(from_ast->getASTContext());
  auto model{sc.getSystemCModel()};
  auto instances{model->getInstances()};

  // There are two module declarations found: test dut sub_module
  CHECK(instances.size() == 3);

  auto found_module_testing{model->getInstance("testing")};
  CHECK(found_module_testing != nullptr);

  auto found_module_submodule{model->getInstance("SUBMODULE")};
  CHECK(found_module_submodule != nullptr);

  SUBCASE("Testing top-level module: test" ) {
    // There should be two modules because there are two instances.
    INFO("Top-level module specified as test.");

    auto found_decl{found_module_testing};
    CHECK(found_decl->getIPorts().size() == 3);
    CHECK(found_decl->getOPorts().size() == 2);
    // This is 4 because sc_buffer is also inheriting from the signal interface.
    CHECK(found_decl->getSignals().size() == 0);
    // 1 non-array, and 2 array others
    CHECK(found_decl->getOtherVars().size() == 0);
    CHECK(found_decl->getNestedModuleInstances().size() == 1);

    // TODO: Check the template parameters.
    //

    // test_module_inst
    found_decl = model->getInstance("dut");
    auto port_bindings{found_decl->getPortBindings()};

    int check_count{3};
    for (auto const &binding : port_bindings) {
      PortBinding *pb{binding.second};
      std::string port_name{pb->getCallerPortName()};
      std::string caller_name{pb->getCallerInstanceName()};
      std::string as_string{pb->toString()};
      llvm::outs() << "check string: " << as_string << "\n";
      if (caller_name == "test_instance") {
        if (port_name == "clk") {
          CHECK(as_string == "test test_instance testing clk clk");
          --check_count;
        }
        if (port_name == "inS") {
          CHECK(as_string == "test test_instance testing inS sig1");
          --check_count;
        }
        if (port_name == "outS") {
          CHECK(as_string == "test test_instance testing outS sig1");
          --check_count;
        }
      }
    }
    CHECK(check_count == 0);
  }

  SUBCASE("Testing submodule") {
    auto found_decl{found_module_submodule};
    CHECK(found_decl->getIPorts().size() == 1);
    CHECK(found_decl->getOPorts().size() == 1);
    CHECK(found_decl->getSignals().size() == 0);
    CHECK(found_decl->getOtherVars().size() == 0);

    auto port_bindings{found_decl->getPortBindings()};
    CHECK(port_bindings.size() == 0);
  }


  /// Instance testing.
  SUBCASE("Test port bindings for instance testing" ) {
    auto found_decl{found_module_testing};
    CHECK(found_decl->getIPorts().size() == 3);
    CHECK(found_decl->getOPorts().size() == 2);
    // This is 4 because sc_buffer is also inheriting from the signal interface.
    CHECK(found_decl->getSignals().size() == 0);
    // 1 non-array, and 2 array others
    CHECK(found_decl->getOtherVars().size() == 0);
    CHECK(found_decl->getNestedModuleInstances().size() == 1);

    /// TODO: Check for submodules, and port bindings.
    auto port_bindings{found_decl->getPortBindings()};
    std::string instance_name{"testing"};
    LLVM_DEBUG(llvm::dbgs() << "Number of port bindings: "
                            << port_bindings.size() << "\n";);

    // Print all the port binding information
    for (auto const &p : port_bindings) {
      auto pb{p.second};
      LLVM_DEBUG(llvm::dbgs() << "bind_name: " << p.first << "\n");
      LLVM_DEBUG(pb->dump(););
    }

    /// Ensure that all checks are encountered.
    int check_count{2};
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
          CHECK(as_string == "submodule sub_module_member SUBMODULE input inS");
          llvm::outs() << "@@@@@ 1st\n";
          --check_count;
        }
        if (port_name == "output") {
          CHECK(as_string == "submodule sub_module_member SUBMODULE output outS");
          --check_count;
        }
      }
    }
    CHECK(check_count == 0);
  }

}



