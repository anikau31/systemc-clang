#include <doctest.h>

#include "SystemCClang.h"

// This is automatically generated from cmake.
#include "../plugins/hdl/HDLMain.h"
#include "ClangArgs.h"
#include "Testing.h"

using namespace systemc_clang;

TEST_CASE("sreg example") {
  std::string code{systemc_clang::read_systemc_file(
      systemc_clang::test_data_dir, "/llnl-examples/find-emax-driver.cpp")};
  INFO(systemc_clang::test_data_dir);

  auto catch_test_args = systemc_clang::catch_test_args;
  catch_test_args.push_back("-I" + systemc_clang::test_data_dir +
                            "/llnl-examples/zfpsynth/shared");

  ASTUnit *from_ast =
      tooling::buildASTFromCodeWithArgs(code, catch_test_args).release();

  /// Set the debug flag on.
  llvm::DebugFlag = true;
  SystemCConsumer sc{from_ast};
  //HDLMain sc{from_ast};
  sc.HandleTranslationUnit(from_ast->getASTContext());
  auto model{sc.getSystemCModel()};
  // These are instances.

 // Testing instances.

  auto mymod{model->getInstance("mymodule_instance")};
  //
  // Begin the tests.
  //
  //
  SUBCASE("Show how to access nested sub-modules") {
    CHECK(mymod != nullptr);

    // Get the nested modules.
    auto nested_mdecls{mymod->getNestedModuleInstances()};
    llvm::outs() << "######################## NESTED SUBMODULE "
                 << nested_mdecls.size() << "\n";

    // Only u_dut should be nested in mymodule.
    CHECK(nested_mdecls.size() == 1);

    for (auto const &mdecl : nested_mdecls) {
      // There is only one, but showing how to access all of them, if there were
      // more than one.
      //
      CHECK(mdecl->getInstanceName() == "u_dut");

      // Check for port bindings
      auto port_bindings{mdecl->getPortBindings()};

      // Print all the port binding information
      for (auto const &p : port_bindings) {
        auto pb{p.second};
        pb->dump();
      }
    }
  }

  auto ureg{model->getInstance("u_reg_ex")};
  SUBCASE("Print out the netlist for instance u_reg_ex") {
    CHECK(ureg != nullptr);

    llvm::outs() << "============= XXXXXXXXXXX ======================\n";
    llvm::outs() << "Print out the sensitivity for u_reg_ex\n";

    llvm::outs() << "############ Process map way \n";
      auto process_map{ureg->getProcessMap()};
      for (const auto &proc: process_map) {
        // Get the EntryFunctionContainer.
        ProcessDecl *pd{get<1>(proc)};
        auto entry_function{pd->getEntryFunction()};
        // Get the sensitivity map.
        auto sense_map{entry_function->getSenseMap()};

        for (auto const &sense : sense_map) {
          llvm::outs() << "sensitivity name: " << sense.first << "\n";
          // There is more to the sensitivity info ... 
        }
      }
  } // Section 

}
