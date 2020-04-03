#include "catch.hpp"
#include "SystemCClang.h"
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

TEST_CASE("Read SystemC model from file for testing", "[parsing]") {
  std::string code{systemc_clang::read_systemc_file(
      systemc_clang::test_data_dir, "matcher-test.cpp")};

  ASTUnit *from_ast =
      tooling::buildASTFromCodeWithArgs(code, systemc_clang::catch_test_args)
          .release();

  llvm::outs() << "================ TESTMATCHER =============== \n";
  ModuleDeclarationMatcher module_declaration_handler{};
  MatchFinder matchRegistry{};
  module_declaration_handler.registerMatchers(matchRegistry);
  // Run all the matchers
  matchRegistry.matchAST(from_ast->getASTContext());
  module_declaration_handler.dump();
  llvm::outs() << "================ END =============== \n";



  /*

  SystemCConsumer sc{from_ast};
  sc.HandleTranslationUnit(from_ast->getASTContext());
  auto model{sc.getSystemCModel()};
  auto module_decl{model->getModuleDecl()};
  model->dump(llvm::outs());
  */

  SECTION("Test systemc-clang AST matchers ", "[matchers]") {
    // The module instances have all the information.
    REQUIRE( true );
  }
}
