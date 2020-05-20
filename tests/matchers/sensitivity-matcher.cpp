#include "SystemCClang.h"
#include "catch.hpp"
#include "clang/Tooling/Tooling.h"

#include "SensitivityMatcher.h"

// This is automatically generated from cmake.
#include "ClangArgs.h"
#include "Testing.h"

using namespace clang::ast_matchers;
using namespace scpar;
using namespace sc_ast_matchers;

template <typename T>
bool find_name(std::vector<T> &names, const T &find_name) {
  auto found_it = std::find(names.begin(), names.end(), find_name);
  if (found_it != names.end()) {
    names.erase(found_it);
    return true;
  }

  return false;
}
// This test works
TEST_CASE("Read SystemC model from file for testing", "[parsing]") {
  std::string code = R"(
#include "systemc.h"

SC_MODULE( test ){

  // input ports
  sc_in_clk clk;
  sc_in<bool> bool_clk;

  void entry_function_1() {
    while(true) {
    }
  }
  SC_CTOR( test ) {
    SC_METHOD(entry_function_1);
    sensitive << clk.pos();
    sensitive << bool_clk;
  }
};


int sc_main(int argc, char *argv[]) {
  test test_instance("testing");

  return 0;
}
     )";

  // std::string code{systemc_clang::read_systemc_file(
  // systemc_clang::test_data_dir, "xor-hierarchy.cpp")};
  //
  ASTUnit *from_ast =
      tooling::buildASTFromCodeWithArgs(code, systemc_clang::catch_test_args)
          .release();

  llvm::outs() << "================ TESTMATCHER =============== \n";
  SensitivityMatcher sens_matcher{};
  MatchFinder matchRegistry{};
  sens_matcher.registerMatchers(matchRegistry);
  // Run all the matchers
  matchRegistry.matchAST(from_ast->getASTContext());
  sens_matcher.dump();
  llvm::outs() << "================ END =============== \n";

  REQUIRE(true);
}
