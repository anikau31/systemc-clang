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

  SECTION("Found sc_modules", "[modules]") {
    REQUIRE(module_decl.size() == 2);

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
