#include "catch.hpp"

#include "SystemCClang.h"
// This is automatically generated from cmake.
#include <iostream>
#include <utility>
#include "ClangArgs.h"

#include "SplitCFG.h"

using namespace systemc_clang;

// Source:
// https://www.toptip.ca/2010/03/trim-leading-or-trailing-white-spaces.html
std::string &trim(std::string &s) {
  size_t p = s.find_first_not_of(" \t");
  s.erase(0, p);

  p = s.find_last_not_of(" \t");
  if (string::npos != p) s.erase(p + 1);

  return s;
}

TEST_CASE("Basic parsing checks", "[parsing]") {
  std::string code = R"(
#include "systemc.h"

SC_MODULE( test ){

  // input ports
  sc_in_clk clk;
  sc_in<int> in1;
  // output ports
  sc_out<int> out1;

  // others
  int x;

  void test_thread() {
    while(true) {
    /*
     x = x+1;
     x = x+2;
     wait();
     out1.write(x);
     x = x +3;
     wait(4);
     wait(4, SC_NS);
     x = x + 6;
     */

    wait(4);
    }
  }

  SC_CTOR( test ) {
   int x{2};
    SC_THREAD(test_thread);
      sensitive << clk.pos();
  }
};

SC_MODULE(DUT) {

  sc_signal<int> sig1;
  sc_signal<double> double_sig;

  test test_instance;


  int others;
  SC_CTOR(DUT) : test_instance("testing") {
  test_instance.in1(sig1);
  test_instance.out1(sig1);
  }


};

int sc_main(int argc, char *argv[]) {
    DUT d("d");
   return 0;
}
     )";

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
    REQUIRE(test_module_inst->getIPorts().size() == 2);
    REQUIRE(test_module_inst->getOPorts().size() == 1);
    REQUIRE(test_module_inst->getIOPorts().size() == 0);
    REQUIRE(test_module_inst->getSignals().size() == 0);
    REQUIRE(test_module_inst->getInputStreamPorts().size() == 0);
    REQUIRE(test_module_inst->getOutputStreamPorts().size() == 0);
    REQUIRE(test_module_inst->getOtherVars().size() == 1);

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
        }
        REQUIRE(check == 0);
      }
    }

    /// Print the CFG for the entry function.
    //

    llvm::outs() << " ********************* CFG ***********************\n";
    /*
    const auto cfg = clang::CFG::buildCFG(method, method->getBody(),
                                          &from_ast->getASTContext(),
                                          clang::CFG::BuildOptions());
    LangOptions lang_opts;
    cfg->dump(lang_opts, true);

    /// Get the root node.
    const clang::CFGBlock &entry{cfg->getEntry()};
    entry.dump();
    */

    SplitCFG scfg{from_ast->getASTContext()};
    scfg.split_wait_blocks( method );
    scfg.dump();
    /*
    /// Access the successor
    for (auto const &succ : entry.succs()) {
      succ->dump();
      /// Try to get the Elements in each CFGBlock
      for (auto const &element : succ->refs()) {
        element->dump();
        if (auto stmt = element->getAs<CFGStmt>()) {
          stmt->getStmt()->dump();
        }
      }
    }

    // Go through all CFG blocks
    llvm::dbgs() << "Iterate through all CFGBlocks.\n";
    /// These iterators are not in clang 12.
    // for (auto const &block: CFG->const_nodes()) {
    for (auto begin_it = CFG->nodes_begin(); begin_it != CFG->nodes_end();
         ++begin_it) {
      auto block = *begin_it;
      block->dump();

      /// Try to split the block.
      SplitCFGBlock sp{};
      sp.split_block(block);
      sp.dump();
    }
    */
  }
}
