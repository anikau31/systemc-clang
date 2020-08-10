#include "catch.hpp"
#include "SystemCClang.h"

#include "Matchers.h"

// This is automatically generated from cmake.
#include "ClangArgs.h"
#include "Testing.h"

using namespace systemc_clang;
using namespace sc_ast_matchers;

TEST_CASE("Only parse a single top-level module", "[parsing]") {
  std::string code{systemc_clang::read_systemc_file(
      systemc_clang::test_data_dir, "ports-arrays.cpp")};
  llvm::DebugFlag = true;

  ASTUnit *from_ast =
      tooling::buildASTFromCodeWithArgs(code, systemc_clang::catch_test_args)
          .release();

  SystemCConsumer sc{from_ast};
  sc.HandleTranslationUnit(from_ast->getASTContext());

  auto model{sc.getSystemCModel()};

  // Want to find an instance named "testing".
  ModuleDecl *pa{model->getInstance("port_array_instance")};;
  if (!pa) { llvm::outs() << "=> ERROR: instance PORT_ARRAY not found\n";}

  SECTION("Testing port_array_instance", "[port arrays]") {
    // Actually found the module.
    REQUIRE(pa != nullptr);

    pa->dump(llvm::outs());
    auto found_decl{pa};
    REQUIRE(found_decl->getIPorts().size() == 2);
    REQUIRE(found_decl->getOPorts().size() == 1);
    REQUIRE(found_decl->getOtherVars().size() == 3);
  }
}
