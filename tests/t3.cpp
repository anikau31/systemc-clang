#include "PluginAction.h"
#include "SystemCClang.h"
#include "catch.hpp"
#include "clang/AST/ASTImporter.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/Parse/ParseAST.h"
#include "clang/Tooling/Tooling.h"

// This is automatically generated from cmake.
#include "ClangArgs.h"
#include "Testing.h"

using namespace clang;
using namespace clang::tooling;
using namespace clang::ast_matchers;
using namespace scpar;

TEST_CASE("Read SystemC model from file for testing", "[parsing]") {
  std::string code{systemc_clang::read_systemc_file(
      systemc_clang::test_data_dir, "basic-module-method.cpp")};

  ASTUnit *from_ast =
      tooling::buildASTFromCodeWithArgs(code, systemc_clang::catch_test_args)
          .release();

  SystemCConsumer sc{from_ast};
  sc.HandleTranslationUnit(from_ast->getASTContext());
  auto model{sc.getSystemCModel()};
  auto module_decl{model->getModuleDecl()};

  SECTION("No ports bound for test declaration", "[ports]") {
    // The module instances have all the information.
    auto test_module{module_decl["test"]};
    // There is only one module instance

    // Check if the proper number of ports are found.
    INFO(
        "FAIL_TEST: A module must have a port bound for it to be "
        "recognized.");
    REQUIRE(test_module != nullptr);
  }
}
