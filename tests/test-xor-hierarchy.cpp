#include "PluginAction.h"
#include "SystemCClang.h"
#include "catch.hpp"
#include "clang/AST/ASTImporter.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/Parse/ParseAST.h"
#include "clang/Tooling/Tooling.h"

#include "Matchers.h"

// This is automatically generated from cmake.
#include "ClangArgs.h"
#include "Testing.h"

using namespace clang;
using namespace clang::tooling;
using namespace clang::ast_matchers;
using namespace scpar;
using namespace sc_ast_matchers;

TEST_CASE("Only parse a single top-level module", "[parsing]") {
  std::string code{systemc_clang::read_systemc_file(
      systemc_clang::test_data_dir, "xor-hierarchy.cpp")};

  ASTUnit *from_ast =
      tooling::buildASTFromCodeWithArgs(code, systemc_clang::catch_test_args)
          .release();

  // Name of top-level module declaration.
  std::string top{"exor2"};

  SystemCConsumer sc{from_ast};
  sc.setTopModule(top);
  sc.HandleTranslationUnit(from_ast->getASTContext());
  auto model{sc.getSystemCModel()};
  auto module_decl{model->getModuleDecl()};

  auto found_module{std::find_if(module_decl.begin(), module_decl.end(),
                                 [&top](const auto &element) {
                                   // Get the declaration name.
                                   return element.second->getName() == top;
                                 })};

  SECTION("Testing top-level module: exor2", "[exor2]") {
    // There should be only one module.
    INFO("Top-level module specified as exor2.");
    REQUIRE(module_decl.size() == 1);

    // Actually found the module.
    REQUIRE(found_module != module_decl.end());

    auto found_decl{found_module->second};
    REQUIRE(found_decl->getIPorts().size() == 2);
    REQUIRE(found_decl->getOPorts().size() == 1);
    REQUIRE(found_decl->getSignals().size() == 3);
    // Other sc_module instances are recognized as others.
    REQUIRE(found_decl->getOtherVars().size() == 4);
  }
}
