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

#include "sc_rvd.h"
#include "rvfifo_cc.h"
template <typename T> using sc_stream = sc_rvd<T>;
template <typename T> using sc_stream_in = sc_rvd_in<T>;
template <typename T> using sc_stream_out = sc_rvd_out<T>;
template <typename T, int IW,  bool RLEV> using sfifo_cc = rvfifo_cc<T, IW, RLEV>;


SC_MODULE( test ){

  // clock ports
  sc_in_clk clk;

  // input ports
  sc_in<bool> bool_clk;
  sc_in<int> another_port;

  // output ports
  sc_out<bool> out_bool;

  // events
  sc_event eve1;
  sc_event eve2;

  // signals
  sc_signal<bool> one_sig;
  sc_signal<int>  two_sig;
  sc_signal<sc_uint<32>> count;

  // sc_rvd?
	sc_stream_in<int> s_fp;
	sc_stream<int> c_fp;

  void entry_function_1() {
    while(true) {
    }
  }
  SC_CTOR( test ) {
    SC_METHOD(entry_function_1);
    sensitive << clk.pos();
    sensitive << bool_clk << another_port ;
    sensitive << out_bool;
    sensitive << eve1 << eve2;
    sensitive << one_sig << two_sig << count;
	  sensitive << s_fp.valid_chg() << s_fp.data_chg();
		sensitive << c_fp.ready_event() << c_fp.ready_event();
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
  // ASTUnit *from_ast =
  // tooling::buildASTFromCodeWithArgs(code, systemc_clang::catch_test_args)
  // .release();
  //
  
  auto catch_test_args = systemc_clang::catch_test_args;
  catch_test_args.push_back("-I" + systemc_clang::test_data_dir +
                            "/llnl-examples/zfpsynth/shared");

  ASTUnit *from_ast =
      tooling::buildASTFromCodeWithArgs(code, catch_test_args).release();

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
