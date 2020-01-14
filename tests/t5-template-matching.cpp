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

TEST_CASE("Read SystemC model from file for testing", "[parsing]") {
  std::string code{systemc_clang::read_systemc_file(
      systemc_clang::test_data_dir, "templated-module.cpp")};

  ASTUnit *from_ast =
      tooling::buildASTFromCodeWithArgs(code, systemc_clang::catch_test_args)
          .release();

  llvm::outs() << "================ TESTMATCHER =============== \n";

  ModuleDeclarationMatcher module_declaration_handler{};
  MatchFinder matchRegistry{};
  module_declaration_handler.registerMatchers(matchRegistry);
  // Run all the matchers
  matchRegistry.matchAST(from_ast->getASTContext());
  module_declaration_handler.pruneMatches();
  module_declaration_handler.dump();

  llvm::outs() << "================ END =============== \n";

  SystemCConsumer sc{from_ast};
  sc.HandleTranslationUnit(from_ast->getASTContext());

  SECTION("Test systemc-clang AST matchers ", "[matchers]") {
    // The module instances have all the information.
    REQUIRE(true);
  }
}

TEST_CASE("Only parse a single top-level module", "[parsing]") {
  std::string code{systemc_clang::read_systemc_file(
      systemc_clang::test_data_dir, "templated-module.cpp")};

  ASTUnit *from_ast =
      tooling::buildASTFromCodeWithArgs(code, systemc_clang::catch_test_args)
          .release();

  llvm::outs() << "================ TESTMATCHER =============== \n";

  // Name of top-level module declaration.
  std::string top{"non_template"};

  ModuleDeclarationMatcher module_declaration_handler{};
  MatchFinder matchRegistry{};
  module_declaration_handler.registerMatchers(matchRegistry);
  // Run all the matchers
  matchRegistry.matchAST(from_ast->getASTContext());
  module_declaration_handler.set_top_module_decl(top);
  module_declaration_handler.pruneMatches();
  module_declaration_handler.dump();

  llvm::outs() << "================ END =============== \n";

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

  SECTION("Testing top-level module: non_template", "[top-module]") {
    // There should be only one module.
    INFO("Top-level module specified as non_template.");
    REQUIRE(module_decl.size() == 1);

    // Actually found the module.
    REQUIRE(found_module != module_decl.end());

    auto found_decl{found_module->second};
    REQUIRE(found_decl->getIPorts().size() == 1);
    REQUIRE(found_decl->getOPorts().size() == 0);
    REQUIRE(found_decl->getOtherVars().size() == 2);
  }
}

TEST_CASE("Testing top-level module: test", "[top-module]") {
  std::string code{systemc_clang::read_systemc_file(
      systemc_clang::test_data_dir, "templated-module.cpp")};

  ASTUnit *from_ast =
      tooling::buildASTFromCodeWithArgs(code, systemc_clang::catch_test_args)
          .release();


  // Name of top-level module declaration.
  std::string top{"test"};

  // llvm::outs() << "================ TESTMATCHER =============== \n";
  // ModuleDeclarationMatcher module_declaration_handler{};
  // MatchFinder matchRegistry{};
  // module_declaration_handler.registerMatchers(matchRegistry);
  // Run all the matchers
  // matchRegistry.matchAST(from_ast->getASTContext());
  // module_declaration_handler.set_top_module_decl(top);
  // module_declaration_handler.pruneMatches();
  // module_declaration_handler.dump();
//
  // llvm::outs() << "================ END =============== \n";

  SystemCConsumer sc{from_ast};
  sc.setTopModule(top);
  sc.HandleTranslationUnit(from_ast->getASTContext());
  auto model{sc.getSystemCModel()};
  auto module_decl{model->getModuleDecl()};

  auto found_module_testing{std::find_if(
      module_decl.begin(), module_decl.end(), [&top](const auto &element) {
        // Get the declaration name.
        return ((element.second->getName() == top) &&
                (element.second->getInstanceName() == "testing"));
      })};

  auto found_module_testing_float{std::find_if(
      module_decl.begin(), module_decl.end(), [&top](const auto &element) {
        // Get the declaration name.
        return ((element.second->getName() == top) &&
                (element.second->getInstanceName() == "testing_float_double"));
      })};

  SECTION("Testing top-level module: test", "[top module]") {
    // There should be two modules because there are two instances.
    INFO("Top-level module specified as test.");
    REQUIRE(module_decl.size() == 2);

    // Actually found the module.
    REQUIRE(found_module_testing != module_decl.end());
    REQUIRE(found_module_testing_float != module_decl.end());

    auto found_decl{found_module_testing->second};
    REQUIRE(found_decl->getIPorts().size() == 4);
    REQUIRE(found_decl->getOPorts().size() == 1);
    // This is 4 because sc_buffer is also inheriting from the signal interface.
    REQUIRE(found_decl->getSignals().size() == 4);
    REQUIRE(found_decl->getOtherVars().size() == 0);

    // TODO: Check the template parameters.
    //

    auto found_decl2{found_module_testing_float->second};
    REQUIRE(found_decl2->getIPorts().size() == 4);
    REQUIRE(found_decl2->getOPorts().size() == 1);
    // 1 regular signal, 2 array signals, 1 sc_buffer, which is a signal too.
    REQUIRE(found_decl2->getSignals().size() == 4);
    REQUIRE(found_decl2->getOtherVars().size() == 0);
    
    // TODO: Check the template parameters.
    //
  }
}
