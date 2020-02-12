#include "catch.hpp"

#include "clang/AST/ASTImporter.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/Parse/ParseAST.h"
#include "clang/Tooling/Tooling.h"

#include "PluginAction.h"
#include "SystemCClang.h"

// This is automatically generated from cmake.
#include <iostream>
#include "ClangArgs.h"

using namespace clang;
using namespace clang::tooling;
using namespace clang::ast_matchers;
using namespace scpar;

///////////////////////////////////////////////////////////////////////////////
//
// Class FindVariableDeclarations
//
//
///////////////////////////////////////////////////////////////////////////////
class FindVariableDeclarations : public MatchFinder::MatchCallback {
 public:
 private:
  std::vector<Decl *> variables_;
  std::map<Decl*, std::vector<DeclRefExpr*>> variable_use_map;

 public:
  void registerMatchers(MatchFinder &finder) {
    /* clang-format off */

    //
    //
    auto match_vardecl = cxxMethodDecl(forEachDescendant(varDecl().bind("vardecl")));
    auto match_declref = cxxMethodDecl(
        forEachDescendant(
          declRefExpr().bind("declref")));

    /* clang-format on */

    // Add the two matchers.
    finder.addMatcher(match_vardecl, this);
    finder.addMatcher(match_declref, this);
  }

  // This is the callback function whenever there is a match.
  virtual void run(const MatchFinder::MatchResult &result) {
    llvm::outs() << "## Matched with SOMETHING\n";

    auto var =
        const_cast<VarDecl *>(result.Nodes.getNodeAs<VarDecl>("vardecl"));
    auto declref = const_cast<DeclRefExpr *>(
        result.Nodes.getNodeAs<DeclRefExpr>("declref"));

    if (var) {
      std::string name{var->getIdentifier()->getNameStart()};
      llvm::outs() << "Found a member variable with name: " << name << "\n";
      // This is where you can insert it into a vector.
      variables_.push_back(var);
    }

    if (declref) {
      llvm::outs() << "Found a variable declref name: \n";
      declref->dump();
      // Get the variable declaration
      declref->getDecl()->dump();
      llvm::outs() << "==> varDecl: " << declref->getDecl() << ": " << declref << "\n";

      // This is the information that can be added into a structure to map the 
      // variable declaration -> reference to declaration.
      // This is perhaps the simplest way to use it.

      // Find the varDecl, if it exists.
      auto variable_decl_it{ variable_use_map.find(declref->getDecl())};
      if ( variable_decl_it != variable_use_map.end() ) {
        variable_decl_it->second.push_back( declref );
      } else {
        std::vector<DeclRefExpr*> references;
        references.push_back( declref );
        variable_use_map.insert( std::pair<Decl*, std::vector<DeclRefExpr*> >(declref->getDecl(), references ));
      }
    }
  }

  void dump() {
    llvm::outs() << "DMP: All the variables\n";
    for (auto const &var : variables_) {
      var->dump();
    }
    llvm::outs() << "DMP: All the references to variables\n";
    for (auto const &var : variable_use_map ) {
      auto vardecl{ var.first };
      auto varref{ var.second };

      llvm::outs() << "== VarDecl \n";
      vardecl->dump();
      for (auto const &use : varref ) {
        llvm::outs() << "  " << use << "\n";
        use->dump();
      }


    }
  }
};

///

TEST_CASE("Basic parsing checks", "[parsing]") {
  std::string code = R"(
#include "systemc.h"

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

  // others
  int x;

  void entry_function_1() {
    while(true) {
      int j;
      int k;

      if (j > 20) {
        int i = 33;
        k = k + i;
        i = i + k + 5;
        x = i;
      } else {
        int i = 7;
        x = i - 1;
        }

    }
  }
  SC_CTOR( test ) {
    SC_METHOD(entry_function_1);
    sensitive << clk.pos();
  }
};

SC_MODULE( simple_module ){

  sc_in_clk clk;
  sc_in<int> one;
  sc_in<int> two;
  sc_out<int> out_one;
  int yx;

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


int sc_main(int argc, char *argv[]) {
  sc_signal<int> sig1;
  sc_signal<double> double_sig;
  test test_instance("testing");
  test_instance.in1(sig1);
  test_instance.in_out(double_sig);
  test_instance.out1(sig1);

  simple_module simple("simple_module_instance");
  simple.one(sig1);

  return 0;
}
     )";

  ASTUnit *from_ast =
      tooling::buildASTFromCodeWithArgs(code, systemc_clang::catch_test_args)
          .release();

  SystemCConsumer sc{from_ast};
  sc.HandleTranslationUnit(from_ast->getASTContext());

  auto model{sc.getSystemCModel()};

  // This provides the module declarations.
  auto module_decl{model->getModuleDecl()};
  auto module_instance_map{model->getModuleInstanceMap()};

  // Want to find an instance named "testing".

  ModuleDecl *test_module{model->getInstance("testing")};
  ;
  ModuleDecl *simple_module{model->getInstance("simple_module_instance")};

  SECTION("Found sc_module instances", "[instances]") {
    // There should be 2 modules identified.
    INFO("Checking number of sc_module declarations found: "
         << module_decl.size());

    REQUIRE(module_decl.size() == 2);

    REQUIRE(test_module != nullptr);
    REQUIRE(simple_module != nullptr);

    INFO("Checking member ports for test instance.");
    // These checks should be performed on the declarations.

    // The module instances have all the information.
    // This is necessary until the parsing code is restructured.
    // There is only one module instance
    // auto module_instances{model->getModuleInstanceMap()};
    // auto p_module{module_decl.find("test")};
    //
    //
    auto test_module_inst{test_module};

    // Check if the proper number of ports are found.
    REQUIRE(test_module_inst->getIPorts().size() == 3);
    REQUIRE(test_module_inst->getOPorts().size() == 2);
    REQUIRE(test_module_inst->getIOPorts().size() == 1);
    REQUIRE(test_module_inst->getSignals().size() == 1);
    REQUIRE(test_module_inst->getOtherVars().size() == 1);
    REQUIRE(test_module_inst->getInputStreamPorts().size() == 0);
    REQUIRE(test_module_inst->getOutputStreamPorts().size() == 0);

    auto entry_functions{test_module_inst->getEntryFunctionContainer()};
    // There should be only one entry function.
    REQUIRE(entry_functions.size() == 1);
    llvm::outs() << "Size of EF: " << entry_functions.size() << "\n";

    // Pick the first one.
    EntryFunctionContainer *entry_function{entry_functions.front()};
    entry_function->dump(llvm::outs(), 0);
    REQUIRE(entry_function != nullptr);

    // Get the member function.
    CXXMethodDecl *method{entry_function->getEntryMethod()};
    method->dump();

    REQUIRE(method != nullptr);

    // Now try to run the subtree AST matcher on this.
    MatchFinder registry{};
    FindVariableDeclarations find_vars{};
    find_vars.registerMatchers(registry);

    llvm::outs() << "Run the matcher\n";
    registry.match(*method, from_ast->getASTContext());

    find_vars.dump();

    INFO("Checking member ports for simple module instance.");
    auto simple_module_inst{simple_module};

    // Check if the proper number of ports are found.
    REQUIRE(simple_module_inst->getIPorts().size() == 3);
    REQUIRE(simple_module_inst->getOPorts().size() == 1);
    REQUIRE(simple_module_inst->getIOPorts().size() == 0);
    REQUIRE(simple_module_inst->getSignals().size() == 0);
    REQUIRE(simple_module_inst->getOtherVars().size() == 1);
    REQUIRE(simple_module_inst->getInputStreamPorts().size() == 0);
    REQUIRE(simple_module_inst->getOutputStreamPorts().size() == 0);
  }
}
