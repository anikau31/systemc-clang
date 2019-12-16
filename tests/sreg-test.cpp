#include "catch.hpp"

#include "clang/AST/ASTImporter.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/Parse/ParseAST.h"
#include "clang/Tooling/Tooling.h"

#include "PluginAction.h"
#include "SystemCClang.h"

// This is automatically generated from cmake.
#include "ClangArgs.h"
#include "Testing.h"

using namespace clang;
using namespace clang::tooling;
using namespace clang::ast_matchers;
using namespace scpar;

TEST_CASE("sreg example",
          "[llnl-examples]") {
  std::string code{systemc_clang::read_systemc_file(
      systemc_clang::test_data_dir, "/llnl-examples/sreg.cpp")};
  INFO(systemc_clang::test_data_dir);

  auto catch_test_args = systemc_clang::catch_test_args;
  catch_test_args.push_back("-I" + systemc_clang::test_data_dir + "/llnl-examples/");

  ASTUnit *from_ast =
      tooling::buildASTFromCodeWithArgs(code, catch_test_args)
          .release();

  SystemCConsumer sc{from_ast};
  sc.HandleTranslationUnit(from_ast->getASTContext());
  auto model{sc.getSystemCModel()};
  auto module_decl{model->getModuleDecl()};

  cout << "MODULE SIZE: " << module_decl.size();
  SECTION("Found sc_modules", "[modules]") {
    INFO( "ERROR: number of sc_module declarations found: " << module_decl.size() );
    CHECK(module_decl.size() == 5);
    auto test_module{
        std::find_if(
            module_decl.begin(), module_decl.end(),
            [](const auto &element) { return element.first == "test"; })};

    auto sreg_module{
        std::find_if(
            module_decl.begin(), module_decl.end(),
            [](const auto &element) { return element.first == "sreg"; })};
    REQUIRE(test_module != module_decl.end() );
    REQUIRE(sreg_module != module_decl.end() );
  }

  SECTION("Found sreg instances", "[instances]") {
    auto module_instances{model->getModuleInstanceMap()};
    //auto p_module{module_decl.find("sreg")};
    auto p_module{
        std::find_if(
            module_decl.begin(), module_decl.end(),
            [](const auto &element) { return element.first == "sreg"; })};
    REQUIRE(module_instances[p_module->second].size() == 3);
  }

  SECTION("Checking sreg_bypass ports", "[ports]") {
    auto module_instances{model->getModuleInstanceMap()};
    //auto p_module{module_decl.find("sreg")};
    auto p_module{
        std::find_if(
            module_decl.begin(), module_decl.end(),
            [](const auto &element) { return element.first == "sreg"; })};

    auto sreg_bypass{module_instances[p_module->second][0]};

    REQUIRE(sreg_bypass->getIPorts().size() == 2);
    REQUIRE(sreg_bypass->getOPorts().size() == 0);
    REQUIRE(sreg_bypass->getIOPorts().size() == 0);
    REQUIRE(sreg_bypass->getSignals().size() == 0);
    REQUIRE(sreg_bypass->getOtherVars().size() == 0);
    REQUIRE(sreg_bypass->getInputStreamPorts().size() == 1);
    REQUIRE(sreg_bypass->getOutputStreamPorts().size() == 1);
  }

  SECTION("Checking sreg_fwd ports", "[ports]") {
    auto module_instances{model->getModuleInstanceMap()};
    //auto p_module{module_decl.find("sreg")};
    auto p_module{
        std::find_if(
            module_decl.begin(), module_decl.end(),
            [](const auto &element) { return element.first == "sreg"; })};
    auto sreg_fwd{module_instances[p_module->second][1]};

    REQUIRE(sreg_fwd->getIPorts().size() == 2);
    REQUIRE(sreg_fwd->getOPorts().size() == 0);
    REQUIRE(sreg_fwd->getIOPorts().size() == 0);
    REQUIRE(sreg_fwd->getSignals().size() == 1);
    REQUIRE(sreg_fwd->getOtherVars().size() == 0);
    REQUIRE(sreg_fwd->getInputStreamPorts().size() == 1);
    REQUIRE(sreg_fwd->getOutputStreamPorts().size() == 1);
  }

  SECTION("Checking sreg_fwd_rev ports", "[ports]") {
    auto module_instances{model->getModuleInstanceMap()};
    //auto p_module{module_decl.find("sreg")};
    auto p_module{
        std::find_if(
            module_decl.begin(), module_decl.end(),
            [](const auto &element) { return element.first == "sreg"; })};
    auto sreg_fwd_rev{module_instances[p_module->second][2]};

    REQUIRE(sreg_fwd_rev->getIPorts().size() == 2);
    REQUIRE(sreg_fwd_rev->getOPorts().size() == 0);
    REQUIRE(sreg_fwd_rev->getIOPorts().size() == 0);
    REQUIRE(sreg_fwd_rev->getSignals().size() == 7);
    REQUIRE(sreg_fwd_rev->getOtherVars().size() == 0);
    REQUIRE(sreg_fwd_rev->getInputStreamPorts().size() == 1);
    REQUIRE(sreg_fwd_rev->getOutputStreamPorts().size() == 1);
  }

  // SECTION("No ports bound", "[ports]") {
  //   // The module instances have all the information.
  //   auto module_instances{model->getModuleInstanceMap()};
  //   auto p_module{module_decl["test"]};
  //   // There is only one module instance
  //   auto test_module{module_instances[p_module].front()};

  //   // Check if the proper number of ports are found.
  //   INFO("FAIL_TEST: A module must have a port bound for it to be recognized.");
  //   REQUIRE(test_module != nullptr);
  // }
}
