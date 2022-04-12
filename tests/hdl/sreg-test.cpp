#include <doctest.h>

#include "SystemCClang.h"

// This is automatically generated from cmake.
#include "../plugins/hdl/HDLMain.h"
#include "ClangArgs.h"
#include "Testing.h"

using namespace systemc_clang;

TEST_CASE("sreg example") {
  std::string code{systemc_clang::read_systemc_file(
      systemc_clang::test_data_dir, "/llnl-examples/sreg-driver.cpp")};
  INFO(systemc_clang::test_data_dir);

  auto catch_test_args = systemc_clang::catch_test_args;
  catch_test_args.push_back("-I" + systemc_clang::test_data_dir +
                            "/llnl-examples/zfpsynth/shared");

  ASTUnit *from_ast =
      tooling::buildASTFromCodeWithArgs(code, catch_test_args).release();

    SystemCConsumer sc{from_ast};
  //HDLMain sc{from_ast};
  sc.HandleTranslationUnit(from_ast->getASTContext());
  auto model{sc.getSystemCModel()};
  // These are instances.
    // Testing instances.

  auto test_module{model->getInstance("testing")};
  auto sreg_bypass{model->getInstance("sreg_bypass")};
  auto sreg_fwd{model->getInstance("sreg_fwd")};
  auto sreg_fwd_rev{model->getInstance("sreg_fwd_rev")};
  //
  // Begin the tests.
  //
  //
  SUBCASE("sreg instance and port tests") {

    CHECK(test_module != nullptr);
    CHECK(sreg_bypass != nullptr);
    CHECK(sreg_fwd != nullptr);
    CHECK(sreg_fwd_rev != nullptr);

    //
    //
    //
    INFO("Checking sreg_bypass ports.");
    auto sreg_bypass_decl{sreg_bypass};

    CHECK(sreg_bypass_decl->getIPorts().size() == 2);
    CHECK(sreg_bypass_decl->getOPorts().size() == 0);
    CHECK(sreg_bypass_decl->getIOPorts().size() == 0);
    CHECK(sreg_bypass_decl->getSignals().size() == 0);
    CHECK(sreg_bypass_decl->getOtherVars().size() == 0);
    CHECK(sreg_bypass_decl->getInputStreamPorts().size() == 1);
    CHECK(sreg_bypass_decl->getOutputStreamPorts().size() == 1);

    //
    //
    //
    INFO("Checking sreg_fwd ports.");
    auto sreg_fwd_decl{sreg_fwd};

    CHECK(sreg_fwd_decl->getIPorts().size() == 2);
    CHECK(sreg_fwd_decl->getOPorts().size() == 0);
    CHECK(sreg_fwd_decl->getIOPorts().size() == 0);
    CHECK(sreg_fwd_decl->getSignals().size() == 1);
    CHECK(sreg_fwd_decl->getOtherVars().size() == 0);
    CHECK(sreg_fwd_decl->getInputStreamPorts().size() == 1);
    CHECK(sreg_fwd_decl->getOutputStreamPorts().size() == 1);

    //
    //
    //
    INFO("Checking sreg_fwd_rev ports.");

    auto sreg_fwd_rev_decl{sreg_fwd_rev};

    CHECK(sreg_fwd_rev_decl->getIPorts().size() == 2);
    CHECK(sreg_fwd_rev_decl->getOPorts().size() == 0);
    CHECK(sreg_fwd_rev_decl->getIOPorts().size() == 0);
    CHECK(sreg_fwd_rev_decl->getSignals().size() == 7);
    // captures the static constexpr
    CHECK(sreg_fwd_rev_decl->getOtherVars().size() == 2);
    CHECK(sreg_fwd_rev_decl->getInputStreamPorts().size() == 1);
    CHECK(sreg_fwd_rev_decl->getOutputStreamPorts().size() == 1);
  }
}
