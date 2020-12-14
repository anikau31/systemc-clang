#include "catch.hpp"
#include "SystemCClang.h"

#include "Matchers.h"

// This is automatically generated from cmake.
#include "ClangArgs.h"
#include "Testing.h"

using namespace systemc_clang;
using namespace sc_ast_matchers;

TEST_CASE("Only parse a single top-level module", "[parsing]") {
  std::string code{systemc_clang::read_systemc_file(
      systemc_clang::test_data_dir, "ports-arrays-input.cpp")};
  llvm::DebugFlag = true;

  ASTUnit *from_ast =
      tooling::buildASTFromCodeWithArgs(code, systemc_clang::catch_test_args)
          .release();

  SystemCConsumer sc{from_ast};
  sc.HandleTranslationUnit(from_ast->getASTContext());

  auto model{sc.getSystemCModel()};

  // Want to find an instance named "testing".
  ModuleInstance *pa{model->getInstance("port_array_instance")};
  if (!pa) {
    llvm::outs() << "=> ERROR: instance PORT_ARRAY not found\n";
  }

  SECTION("Testing port_array_instance", "[port arrays]") {
    // Actually found the module.
    REQUIRE(pa != nullptr);

    auto found_decl{pa};
    REQUIRE(found_decl->getIPorts().size() == 3);

    /// Check the port bindings for the input ports.
    ///
    auto port_bindings{pa->getPortBindings()};
    std::vector<std::string> test_ports{"a"};

    llvm::dbgs() << "# of port bindings: " << port_bindings.size() << "\n";

    int check_count{1};
    for (auto const &binding : port_bindings) {
      PortBinding *pb{binding.second};
      std::string caller_name{pb->getCallerInstanceName()};
      std::string port_name{pb->getCallerPortName()};
      std::string as_string{pb->toString()};

      llvm::outs() << "\n";
      llvm::outs() << "check string: " << as_string << "\n";
      llvm::outs() << "inst name: " << pb->getCallerInstanceName()
                   << ", port name: " << pb->getCallerPortName() << "\n\n";

      if (caller_name == "submodules_3d") {
        if (port_name == "clk") {
          REQUIRE(as_string ==
                  "lift submodules_3d submod_0_0_0 0 0 0 clk test");
          --check_count;
        }
      }

      REQUIRE(check_count == 0);
    }

    pa = model->getInstance("design_under_test");

    if (!pa) {
      llvm::outs() << "=> ERROR: instance design_under_test not found\n";
    }

    // Actually found the module.
    REQUIRE(pa != nullptr);

    found_decl = pa;
    REQUIRE(found_decl->getIPorts().size() == 0);
    REQUIRE(found_decl->getOPorts().size() == 0);
    REQUIRE(found_decl->getOtherVars().size() == 0);
    REQUIRE(found_decl->getNestedModuleInstances().size() == 1);

    /// Check the port bindings for the input ports.
    ///
    port_bindings = pa->getPortBindings();

    llvm::dbgs() << "# of port bindings: " << port_bindings.size() << "\n";

    for (auto const &binding : port_bindings) {
      PortBinding *pb{binding.second};
      std::string caller_name{pb->getCallerInstanceName()};
      std::string port_name{pb->getCallerPortName()};
      std::string as_string{pb->toString()};

      llvm::outs() << "\n";
      llvm::outs() << "check string: " << as_string << "\n";
      llvm::outs() << "inst name: " << pb->getCallerInstanceName()
                   << ", port name: " << pb->getCallerPortName() << "\n\n";

      int check_count{2};
      if (caller_name == "par") {
        if (port_name == "a") {
          REQUIRE(as_string == "ports_arrays par port_array_instance a x as z");
          --check_count;
        }
        if (port_name == "test") {
          REQUIRE(as_string == "ports_arrays par port_array_instance test ts");
          --check_count;
        }
      }
    }
    REQUIRE(check_count == 0);

    /*
    for (auto const &pname : test_ports) {
      auto found_it{port_bindings.find(pname)};
      // Actually found the name
      REQUIRE(found_it != port_bindings.end());
      // Check now if the names
      std::string port_name{found_it->first};
      PortBinding *binding{found_it->second};
      std::string as_string{binding->toString()};

      // Only checking the array port.
      if (port_name == "a") {
        llvm::outs() << as_string << "\n";
        REQUIRE(as_string == "ports_arrays par port_array_instance as");

        /// Check if the PortBinding is an array.
        DeclRefExpr *port_member_array_idx{ binding->getPortArrayIndex()};
        DeclRefExpr *port_bound_to_array_idx{
    binding->getBoundToArrayIndex()}; REQUIRE(port_member_array_idx !=
    nullptr); REQUIRE(port_bound_to_array_idx != nullptr);

        LLVM_DEBUG(llvm::dbgs() << "Should be a: " << port_name << "\n"; );
        LLVM_DEBUG(llvm::dbgs() << "Should be as: " <<
    binding->getBoundToName()
    << "\n"; ); LLVM_DEBUG(llvm::dbgs() << "Should be x: " <<
    port_member_array_idx->getNameInfo().getName().getAsString() << "\n"; );
        LLVM_DEBUG(llvm::dbgs() << "Should be z: " <<
    port_bound_to_array_idx->getNameInfo().getName().getAsString() << "\n"; );

        REQUIRE(port_member_array_idx->getNameInfo().getName().getAsString()
    == "x");
    REQUIRE(port_bound_to_array_idx->getNameInfo().getName().getAsString()
    == "z");
      }
    }
    */

  }
}
