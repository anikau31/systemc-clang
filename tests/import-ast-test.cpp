#include "catch.hpp"

#include "clang/AST/ASTImporter.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/Tooling/Tooling.h"
#include "Matchers.h"

using namespace scpar;
using namespace tooling;
using namespace ast_matchers;
using namespace sc_ast_matchers;

template <typename Node, typename Matcher>
Node *getFirstDecl(Matcher M, const std::unique_ptr<ASTUnit> &Unit) {
  auto MB = M.bind("bindStr"); // Bind the to-be-matched node to a string key.
  auto MatchRes = match(MB, Unit->getASTContext());
  // We should have at least one match.
  assert(MatchRes.size() >= 1);
  // Get the first matched and bound node.
  Node *Result =
      const_cast<Node *>(MatchRes[0].template getNodeAs<Node>("bindStr"));
  assert(Result);
  return Result;
}

class ModuleMatcher: public MatchFinder::MatchCallback {
  public:
    void registerMatchers( MatchFinder &finder ) {
      found_module_ = nullptr;
      // Match simple_module
      auto match_simple_module = 
        // clang-format off
        cxxRecordDecl(
            isExpansionInMainFile(),
            isDerivedFrom(
              hasName("::sc_core::sc_module") 
              ),
            hasName("simple_module")
            );      
      //clang-format on
      finder.addMatcher( match_simple_module.bind( "simple_module"), this );

    }

    virtual void run( const MatchFinder::MatchResult &result ) {
      if ( auto module_decl = const_cast<CXXRecordDecl*>(result.Nodes.getNodeAs<CXXRecordDecl>("simple_module")) ) {
        cout << " Found the one module: " << module_decl->getIdentifier()->getNameStart() << endl;
        std::string name{ module_decl->getIdentifier()->getNameStart() };
        found_module_ = module_decl;
      }
    }

    CXXRecordDecl* getFoundModule() const { return found_module_; }
  private: 
    CXXRecordDecl* found_module_;
};

TEST_CASE( "Matchers and import of AST", "[matchers-import]") {
 std::string code = R"(
#include "systemc.h"

SC_MODULE( test ){

  sc_in_clk clk;
  sc_in<int> in1;
  sc_in<int> in2;
  /*
   * Template imports seem to fail.  Still have to see.
  sc_inout<double> in_out;
  sc_out<int> out1;
  sc_out<int> out2;
  sc_signal<int> internal_signal;
  */

  int x;

  void entry_function_1() {
    while(true) {
    }
  }
  SC_CTOR( test ) {
    SC_METHOD(entry_function_1);
    sensitive << clk.pos();
  }
};

    class MyClass {
      int m1;
      int m2;
    };
    )";

  /*
   std::string code = R"(
#include "systemc.h"

SC_MODULE( test ){

  sc_in_clk clk;
  sc_in<int> in1;
  sc_in<int> in2;
  sc_inout<double> in_out;
  sc_out<int> out1;
  sc_out<int> out2;
  sc_signal<int> internal_signal;

  int x;

  void entry_function_1() {
    while(true) {
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
  sc_out<int> out_one;
  int yx;

  void entry_function_1() {
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

  simple_module simple("simple_second_module");
  return 0;
}
     )";
     */

  std::vector<std::string> args{
    // TODO: Generalize these includes.
    // Perhaps there is a way for cmake to generate these.
    "-D__STD_CONSTANT_MACROS", 
      "-D__STDC_LIMIT_MACROS", 
      "-I/home/twiga/bin/clang-9.0.0/include",
      "-I/home/twiga/bin/clang-9.0.0/lib/clang/9.0.0/include", 
      "-I/home/twiga/code/systemc-2.3.3/systemc/include/", 
      "-I/home/twiga/bin/clang-9.0.0/include", 
      "-std=c++14"
  };

std::unique_ptr<ASTUnit> from_ast = tooling::buildASTFromCodeWithArgs(
    code, 
    args );

// Run all the matchers

//ModuleMatcher  module_declaration_handler{}; 
//MatchFinder matchRegistry{};
//module_declaration_handler.registerMatchers( matchRegistry );

auto Matcher = cxxRecordDecl(hasName("test"));
auto *From = getFirstDecl<CXXRecordDecl>(Matcher, from_ast);

SECTION( "Check if MyClass found" ) {

//  matchRegistry.matchAST( from_ast->getASTContext() );
  
  // The simple_module should be found.
  REQUIRE( From != nullptr );
}

SECTION( "Test import of AST", "[import-ast]") {

// Create an empty AST.
std::unique_ptr<ASTUnit> to_unit = buildASTFromCode("", "to.cc");
ASTImporter importer( to_unit->getASTContext(),  to_unit->getFileManager(), 
    from_ast->getASTContext(),  from_ast->getFileManager(), true );

from_ast->enableSourceFileDiagnostics();
to_unit->enableSourceFileDiagnostics();

  llvm::errs() << "Perform the import\n";
  llvm::Expected<Decl*> ImportedOrErr = 
    importer.Import( From );//module_declaration_handler.getFoundModule() );

  llvm::errs() << "Check error on import\n";
  if (!ImportedOrErr) {
  //  llvm::Error Err = ImportedOrErr.takeError();
    llvm::errs() << "ERROR: \n";
   // consumeError(std::move(Err));
  } else { llvm::errs() << "No error\n"; }

  Decl *Imported = *ImportedOrErr;
  Imported->getTranslationUnitDecl()->dump();


  if (llvm::Error Err = importer.ImportDefinition( From ) ) {
    //module_declaration_handler.getFoundModule() )) {
   Err = ImportedOrErr.takeError();
    llvm::errs() << "ERROR 2: " << Err << "\n";
    consumeError(std::move(Err));
  }
  llvm::errs() << "################################### Imported definition.\n";
  llvm::errs() << "Imported definition.\n";
  Imported->getTranslationUnitDecl()->dump();

  REQUIRE( true );
}
}
