#include <doctest.h>

#include "SystemCClang.h"
// This is automatically generated from cmake.
#include <iostream>
#include "ClangArgs.h"

#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace clang;
using namespace clang::ast_matchers;

using namespace sc_ast_matchers::utils;
using namespace systemc_clang;

class ForLoopMatcher : public MatchFinder::MatchCallback {
 public:
   auto makeLoopMatcher(llvm::StringRef name, bool nested = true) {

    auto InitVar =
        hasLoopInit(declStmt(hasSingleDecl(varDecl().bind(name.str() + "_init_vd"))));
    auto CondVar = 
      hasCondition(binaryOperator(hasLHS(declRefExpr(to(varDecl().bind(name.str() + "_cond_vd"))))));
    auto IncVar = 
      hasIncrement(unaryOperator(hasUnaryOperand(declRefExpr(to(varDecl().bind(name.str() + "_inc_vd"))))));

    if (nested) {
      return forStmt(InitVar, CondVar, IncVar).bind(name);
    } else {
      return forStmt(InitVar, CondVar, IncVar, unless(hasDescendant(forStmt())), unless(hasAncestor(forStmt()))).bind(name);
    }
   }

  void registerMatchers(MatchFinder &finder) {
    /* clang-format off */
    auto TestClass = cxxRecordDecl(hasName("test")).bind("cxx_decl");
    auto ForLoop = cxxRecordDecl(hasName("test"),
                                 forEachDescendant(forStmt().bind("for_stmt")))
                       .bind("cxx_decl");
    auto LoopInLoop =
        forStmt(hasDescendant(makeLoopMatcher("nested"))).bind("for_stmt");

    auto NestedForLoop =
        cxxRecordDecl(hasName("test"), forEachDescendant(LoopInLoop))
            .bind("cxx_decl");

    auto NotNestedForLoop = 
        cxxRecordDecl(hasName("test"), 
            forEachDescendant(makeLoopMatcher("notnested", false))
            ).bind("cxx_decl");
        // cxxRecordDecl(hasName("test"), forEachDescendant(forStmt(unless(hasDescendant(forStmt())),
            // unless(hasAncestor(forStmt()))
            // ).bind("notnested_for"))).bind("cxx_decl");
//
    auto cxxdecl = cxxRecordDecl(hasName("test")).bind("cxx_decl");

    /* clang-format on */

    finder.addMatcher(traverse(TK_IgnoreUnlessSpelledInSource, NestedForLoop),
                      this);
    finder.addMatcher(
        traverse(TK_IgnoreUnlessSpelledInSource, NotNestedForLoop), this);
  }

  virtual void run(const MatchFinder::MatchResult &Result) {
    const clang::CXXRecordDecl *cxx_decl=
        Result.Nodes.getNodeAs<clang::CXXRecordDecl>("cxx_decl");
    const clang::ForStmt *parent =
        Result.Nodes.getNodeAs<clang::ForStmt>("for_stmt");
    const clang::ForStmt *nested =
        Result.Nodes.getNodeAs<clang::ForStmt>("nested");
    const clang::ForStmt *not_nested =
        Result.Nodes.getNodeAs<clang::ForStmt>("notnested");

    const clang::VarDecl *nn_init_vd=
        Result.Nodes.getNodeAs<clang::VarDecl>("notnested_init_vd");

    if (cxx_decl && parent && nested) {
      llvm::dbgs() << "##### \n For Stmt\n";
      // parent->dump();

      llvm::dbgs() << "#Parent Loop\n";
      parent->getInit()->dump();

      llvm::dbgs() << "#Nested loop\n";
      nested->getInit()->dump();
    }

    if (cxx_decl && not_nested) {
      llvm::dbgs() << "#Not nested Loop\n";
      not_nested->getInit()->dump();
      if (nn_init_vd) {
      llvm::dbgs() << "#INIT\n";
        nn_init_vd->dump();
      }
    }
  }
};

TEST_CASE("Basic parsing checks") {
  std::string code = R"(
#include "systemc.h"

struct A {
  sc_uint<4> data_;

  void setData(const sc_uint<4> & d ) { data_ = d; }
  sc_uint<4> getData() const { return data_; }
  void compute() {
    setData(2);
    sc_uint<4> x = getData();
  }
};

SC_MODULE( test ){

  // input ports
  sc_in_clk clk;
  sc_in<int> in1;
  sc_in<int> in2;
  // inout ports
  sc_inout<double> in_out;
  // output ports
  sc_out<int> out1;
  sc_out<int> out2;
  //signals
  sc_signal<int> internal_signal;

  sc_in<int> *p_in;
  sc_signal<int> *p_sig;
  sc_out<int> *p;


  // others
  int x;

  /// Test for array of ports and signals.
  //
  static constexpr int MAX_DEPTH = 4;
  const unsigned depth = MAX_DEPTH;

  sc_signal<int> data[MAX_DEPTH];

  sc_out<double> out_array_port[MAX_DEPTH+1];

  // 2d and 3d array
  sc_out<sc_uint<2>> two_dim[2][3];
  sc_in<sc_uint<2>> three_dim[2][3][4];

  sc_signal<sc_uint<2>> two_dim_sig[2][3];
  sc_signal<sc_uint<2>> three_dim_sig[2][3][4];

  void entry_function_1() {
    while(true) {
     x = x+1;
     out1.write(x);
    }
  }
  SC_CTOR( test ) {
   int x{2};
    SC_METHOD(entry_function_1);
    sensitive << clk.pos();
    // sensitive << out_array_port[x -1] << data[x + 2];
    // Sensitivity
    for (int i{0}; i <= MAX_DEPTH; ++i) {
      sensitive << out_array_port[i];
      for (int j{0}; j <= MAX_DEPTH+1; ++j) {
        sensitive << out_array_port[j];

        for (int k{0}; k <= MAX_DEPTH+2; ++k) {
          sensitive << out_array_port[k];
          for (int k1{0}; k1 <= MAX_DEPTH+2; ++k1) {
          sensitive << out_array_port[k1];


        }
   

        }
      }
    }

    for (int l{0}; l <= MAX_DEPTH+2; ++l) {
          sensitive << out_array_port[l];
        }
     
  }
};

SC_MODULE( simple_module ){

  sc_in_clk clk;
  sc_in<int> one;
  sc_in<int> two;
  sc_out<int> out_one;
  int yx;

  // pointer
  char* str;

  void entry_function_1() {
    int x_var;
    double y_var;
    sc_int<4> z_var;
    while(true) {
    }
  }

  SC_CTOR( simple_module ) {
    SC_METHOD(entry_function_1);
    sensitive << clk.pos();
  }
};

SC_MODULE(DUT) {

  sc_signal<int> sig1;
  sc_signal<double> double_sig;

  test test_instance;

  simple_module simple;

  int others;
  SC_CTOR(DUT) : test_instance("testing"), simple("simple_module_instance") {
  test_instance.in1(sig1);
  test_instance.in_out(double_sig);
  test_instance.out1(sig1);

  simple.one(sig1);
  }


};

int sc_main(int argc, char *argv[]) {
  DUT d("d");
   return 0;
}
     )";

  ASTUnit *from_ast =
      tooling::buildASTFromCodeWithArgs(code, systemc_clang::catch_test_args)
          .release();

  ForLoopMatcher matcher;
  MatchFinder finder;
  matcher.registerMatchers(finder);
  //  finder.addMatcher(DeclMatcher, &matcher);
  finder.matchAST(from_ast->getASTContext());
}
