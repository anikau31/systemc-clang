#include "catch.hpp"

#include "SystemCClang.h"
// This is automatically generated from cmake.
#include <iostream>
#include <utility>
#include "ClangArgs.h"
#include "Testing.h"

#include "SplitCFG.h"

using namespace systemc_clang;
TEST_CASE("Simple thread test", "[threads]") {
  std::string code{systemc_clang::read_systemc_file(
      systemc_clang::test_data_dir, "simple-thread-input.cpp")};

  ASTUnit *from_ast =
      tooling::buildASTFromCodeWithArgs(code, systemc_clang::catch_test_args)
          .release();

  /// Turn debug on
  //
  llvm::DebugFlag = true;
  // llvm::setCurrentDebugType("SensitivityMatcher");

  SystemCConsumer systemc_clang_consumer{from_ast};
  systemc_clang_consumer.HandleTranslationUnit(from_ast->getASTContext());

  auto model{systemc_clang_consumer.getSystemCModel()};

  // This provides the module declarations.
  auto instances{model->getInstances()};

  // Want to find an instance named "testing".

  ModuleInstance *test_module{model->getInstance("testing")};
  ModuleInstance *dut{model->getInstance("d")};

  SECTION("Found sc_module instances", "[instances]") {
    // There should be 2 modules identified.
    INFO("Checking number of sc_module instances found: " << instances.size());

    REQUIRE(instances.size() == 2);

    REQUIRE(test_module != nullptr);

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
    REQUIRE(test_module_inst->getOPorts().size() == 1);
    REQUIRE(test_module_inst->getIOPorts().size() == 0);
    REQUIRE(test_module_inst->getSignals().size() == 0);
    REQUIRE(test_module_inst->getInputStreamPorts().size() == 0);
    REQUIRE(test_module_inst->getOutputStreamPorts().size() == 0);
    REQUIRE(test_module_inst->getOtherVars().size() == 2);

    // Check process information
    //

    // processMapType
    auto process_map{test_module_inst->getProcessMap()};
    REQUIRE(process_map.size() == 1);

    const auto proc_decl{*process_map.begin()};
    const auto entry_func{proc_decl.second->getEntryFunction()};
    const auto method{entry_func->getEntryMethod()};

    for (auto const &proc : process_map) {
      auto entry_func{proc.second->getEntryFunction()};
      if (entry_func) {
        auto sense_map{entry_func->getSenseMap()};
        REQUIRE(sense_map.size() == 1);

        int check{1};
        for (auto const &sense : sense_map) {
          if ((sense.first == "test_thread_handle__clkpos")) --check;
          if ((sense.first == "simple_wait_handle__clkpos")) --check;
        }
        REQUIRE(check == 0);
      }
    }

    /// Print the CFG for the entry function.
    //

    llvm::outs() << " ********************* CFG ***********************\n";
    SplitCFG scfg{from_ast->getASTContext()};
    //scfg.split_wait_blocks(method);
    // scfg.build_sccfg( method );
    //scfg.generate_paths();
    scfg.construct_sccfg(method);
    scfg.generate_paths();
    scfg.dump();
    scfg.dumpToDot();

  }
}
