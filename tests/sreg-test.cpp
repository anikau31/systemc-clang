#include "catch.hpp"

#include "SystemCClang.h"

// This is automatically generated from cmake.
#include "../plugins/xlat/Xlat.h"
#include "ClangArgs.h"
#include "Testing.h"

using namespace scpar;

TEST_CASE("sreg example", "[llnl-examples]") {
  std::string code{systemc_clang::read_systemc_file(
      systemc_clang::test_data_dir, "/llnl-examples/sreg-driver.cpp")};
  INFO(systemc_clang::test_data_dir);

  auto catch_test_args = systemc_clang::catch_test_args;
  catch_test_args.push_back("-I" + systemc_clang::test_data_dir +
                            "/llnl-examples/zfpsynth/shared");

  ASTUnit *from_ast =
      tooling::buildASTFromCodeWithArgs(code, catch_test_args).release();

    SystemCConsumer sc{from_ast};
  //Xlat sc{from_ast};
  sc.HandleTranslationUnit(from_ast->getASTContext());
  auto model{sc.getSystemCModel()};
  // These are instances.
  auto module_decl{model->getModuleDecl()};

  llvm::outs() << "\n";
  for (const auto &decl : module_decl) {
    llvm::outs() << "[ " << decl.first << "    ]" << decl.second << "   "
                 << decl.second->getInstanceName() << "\n";
  }

  // Testing instances.

  auto test_module{model->getInstance("testing")};
  auto sreg_bypass{model->getInstance("sreg_bypass")};
  auto sreg_fwd{model->getInstance("sreg_fwd")};
  auto sreg_fwd_rev{model->getInstance("sreg_fwd_rev")};
  //
  // Begin the tests.
  //
  //
  SECTION("sreg instance and port tests", "[instances]") {
    INFO("ERROR: number of sc_module declarations found: "
         << module_decl.size());
    CHECK(module_decl.size() == 4);

    REQUIRE(test_module != nullptr);
    REQUIRE(sreg_bypass != nullptr);
    REQUIRE(sreg_fwd != nullptr);
    REQUIRE(sreg_fwd_rev != nullptr);

    //
    //
    //
    INFO("Checking sreg_bypass ports.");
    auto sreg_bypass_decl{sreg_bypass};

    REQUIRE(sreg_bypass_decl->getIPorts().size() == 2);
    REQUIRE(sreg_bypass_decl->getOPorts().size() == 0);
    REQUIRE(sreg_bypass_decl->getIOPorts().size() == 0);
    REQUIRE(sreg_bypass_decl->getSignals().size() == 0);
    REQUIRE(sreg_bypass_decl->getOtherVars().size() == 0);
    REQUIRE(sreg_bypass_decl->getInputStreamPorts().size() == 1);
    REQUIRE(sreg_bypass_decl->getOutputStreamPorts().size() == 1);

    //
    //
    //
    INFO("Checking sreg_fwd ports.");
    auto sreg_fwd_decl{sreg_fwd};

    REQUIRE(sreg_fwd_decl->getIPorts().size() == 2);
    REQUIRE(sreg_fwd_decl->getOPorts().size() == 0);
    REQUIRE(sreg_fwd_decl->getIOPorts().size() == 0);
    REQUIRE(sreg_fwd_decl->getSignals().size() == 1);
    REQUIRE(sreg_fwd_decl->getOtherVars().size() == 0);
    REQUIRE(sreg_fwd_decl->getInputStreamPorts().size() == 1);
    REQUIRE(sreg_fwd_decl->getOutputStreamPorts().size() == 1);

    //
    //
    //
    INFO("Checking sreg_fwd_rev ports.");

    auto sreg_fwd_rev_decl{sreg_fwd_rev};

    REQUIRE(sreg_fwd_rev_decl->getIPorts().size() == 2);
    REQUIRE(sreg_fwd_rev_decl->getOPorts().size() == 0);
    REQUIRE(sreg_fwd_rev_decl->getIOPorts().size() == 0);
    REQUIRE(sreg_fwd_rev_decl->getSignals().size() == 7);
    // captures the static constexpr
    REQUIRE(sreg_fwd_rev_decl->getOtherVars().size() == 2);
    REQUIRE(sreg_fwd_rev_decl->getInputStreamPorts().size() == 1);
    REQUIRE(sreg_fwd_rev_decl->getOutputStreamPorts().size() == 1);
  }
}
