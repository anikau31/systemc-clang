#include "SystemCClang.h"
#include "catch.hpp"
#include "clang/Tooling/Tooling.h"

#include "SensitivityMatcher.h"

// This is automatically generated from cmake.
#include "ClangArgs.h"
#include "Testing.h"

using namespace clang::ast_matchers;
using namespace systemc_clang;
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

std::string generateSensitivityName(
    const std::vector<SensitivityMatcher::SensitivityTupleType> &sense_args) {
  std::string name{};
  for (auto const entry : sense_args) {
    name = name + std::get<0>(entry);
  }

  return name;
}

// This test works
TEST_CASE("Read SystemC model from file for testing", "[parsing]") {
  std::string code = R"(
#include "systemc.h"

#include "rvfifo_cc.h"
#include "sc_rvd.h"
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

  void second_ef() {
  }

  SC_CTOR( test ) {
    SC_METHOD(entry_function_1);
    sensitive << clk.pos();
    sensitive << bool_clk << another_port ;
    sensitive << out_bool;
    sensitive << eve1 ;
    sensitive << eve2 ;
    sensitive << one_sig << two_sig << count;

	  sensitive << s_fp.valid_chg() << s_fp.data_chg();
		sensitive << c_fp.ready_event(); 

    SC_METHOD(second_ef);
    sensitive << clk.pos();
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

  /// Turn on debug
  //
  llvm::DebugFlag = true;

  ASTUnit *from_ast =
      tooling::buildASTFromCodeWithArgs(code, catch_test_args).release();
  //
  // llvm::outs() << "================ TESTMATCHER =============== \n";
  // SensitivityMatcher sens_matcher{};
  // MatchFinder matchRegistry{};
  // sens_matcher.registerMatchers(matchRegistry);
  // matchRegistry.matchAST(from_ast->getASTContext());
  // sens_matcher.dump();
  // llvm::outs() << "================ END =============== \n";
  //

  SystemCConsumer sc{from_ast};
  sc.HandleTranslationUnit(from_ast->getASTContext());

  auto model{sc.getSystemCModel()};

  // This provides the module declarations.
  auto module_decl{model->getModuleDecl()};
  auto module_instance_map{model->getModuleInstanceMap()};

  REQUIRE(module_instance_map.size() == 1);
  // Want to find an instance named "testing".

  ModuleDecl *test_module{model->getInstance("testing")};
  auto processes{test_module->getProcessMap()};
  auto first_proc{processes.begin()};
  ProcessDecl *proc{first_proc->second};

  REQUIRE(test_module != nullptr);
  REQUIRE(processes.size() == 2);
  REQUIRE(first_proc != processes.end());

  llvm::outs() << "PROCESS: " << first_proc->first << "\n";

  // Get access to the sensitivity list.
  EntryFunctionContainer *ef{proc->getEntryFunction()};
  EntryFunctionContainer::SenseMapType sensitivity_list{ef->getSenseMap()};

  std::vector<std::string> arg_names{
      "another_port", "bool_clk",  "c_fpreadyvalue_changed_event",
      "clkpos",       "count",     "eve1",
      "eve2",         "one_sig",   "out_bool",
      "s_fpdata",     "s_fpvalid", "two_sig"};

  for (auto const &arg : sensitivity_list) {
    // This is a concatenated string of all the call arguments
    // Assumption is that they are unique, but they don't have to be I guess.
    auto name{arg.first};
    // This is a vector of tuples
    auto entry{arg.second};

    llvm::outs() << name << "\n";
    find_name(arg_names, generateSensitivityName(entry));

    /// The tuple now has the last element std::get<3>(.) that provides a
    /// DeclRfExpr to the process_handle. This means that the sensitivity has to
    /// be extracted in relation to what the DeclRefExpr is.  Thus, there is no
    /// separate structure to hold DeclRefExpr => sensitivity list. If needed,
    /// we can add that.

    for (auto const &call : entry) {
      DeclRefExpr *to_get_process_handle{std::get<3>(call)};
      auto process_handle_name{
          to_get_process_handle->getNameInfo().getAsString()};
      llvm::outs() << process_handle_name << "  " << std::get<0>(call) << "  "
                   << std::get<1>(call) << "  " << std::get<2>(call) << "  "
                   << std::get<3>(call) << "\n";
    }
  }

  // Make sure that all of the ports were found.
  REQUIRE(arg_names.size() == 0);
}
