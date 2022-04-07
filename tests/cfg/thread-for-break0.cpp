#include "SystemCClang.h"
// This is automatically generated from cmake.
#include <iostream>
#include <utility>
#include "ClangArgs.h"
#include "Testing.h"

#include "SplitCFG.h"
#include "SplitCFGBlock.h"

#include "catch.hpp"

using namespace systemc_clang;

/// Convert path block ids to string for testing.
template <typename S, typename T>
std::string pathToString(const llvm::SmallVectorImpl<std::pair<S, T> > &v) {
  std::string str{};
  std::size_t sz{v.size()};
  for (auto const &e : v) {
    std::string id{std::to_string(e.first->getBlockID())};
    str += id;
    if (sz != 1) {
      str += " ";
    }
    --sz;
  }
  return str;
}

extern std::string data_file;
TEST_CASE("Simple thread test", "[threads]") {
  std::string code{};

  if (data_file.empty()) {
    code = systemc_clang::read_systemc_file(systemc_clang::test_data_dir,
                                            "thread-for-break0-input.cpp");
  } else {
    code = systemc_clang::read_systemc_file(systemc_clang::test_data_dir,
                                            data_file);
  }

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

    REQUIRE(instances.size() >= 2);

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

    // Check process information
    //

    // processMapType
    auto process_map{test_module_inst->getProcessMap()};
    REQUIRE(process_map.size() != 0);

    for (auto const &proc : process_map) {
      const auto proc_decl{proc};
      const auto entry_func{proc_decl.second->getEntryFunction()};
      const auto method{entry_func->getEntryMethod()};

      /// Print the CFG for the entry function.
      //

      llvm::dbgs() << " ********************* CFG " << proc.first
                   << " ***********************\n";
      SplitCFG scfg{from_ast->getASTContext()};
      scfg.construct_sccfg(method);
      scfg.generate_paths();
      scfg.dumpToDot();
      llvm::dbgs() << " ===================================================\n";

      /// Check if all paths are correct.
      /// These have been worked out by hand.
      unsigned int i{0};
      for (const auto &p : scfg.getPathsFound()) {
        /// There should be 4 paths
        std::string pstr{pathToString(p)};
        if (i == 0) {
          REQUIRE(pstr == "13 12 121");
        }
        if (i == 1) {
          REQUIRE(pstr == "11 10 9 8 7 71 6 2 21 2 21");
        }
        if (i == 2) {
          REQUIRE(pstr == "4 41");
        }
        if (i == 3) {
          REQUIRE(pstr == "22 1 11 10 9 8 7 71 6 2 21 2 21");
        }
        if (i == 4) {
          REQUIRE(pstr == "42 3 9 8 7 71 6 2 21 2 21");
        }
        ++i;
      }
      /// 4 Paths
      REQUIRE(i == 5);

      /// Check if the TRUE/FALSE paths are correct.
      auto path_info{scfg.getPathInfo()};
      int check{2};
      for (const auto &block : path_info) {
        auto sblock{block.first};
        auto info{block.second};
        auto id{ sblock->getBlockID()};
        std::string tstr{info.toStringTruePath()};
        std::string fstr{info.toStringFalsePath()};

        if (id == 9) {
          REQUIRE(tstr == "8");
          REQUIRE(fstr == "2 21");
          --check;
        }

        if (id == 8) {
          REQUIRE(tstr == "7 71" );
          REQUIRE(fstr == "6 2 21" );
          --check;
        }
      }

      REQUIRE(check == 0);


    }

    llvm::outs() << "data_file: " << data_file << "\n";
  }
}
