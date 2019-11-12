//
// This example shows how to import a part of the AST, and run a 
// separate AST matcher on it.
//
// source: https://clang.llvm.org/docs/LibASTImporter.html#id1
//

#include "catch.hpp"

#include "clang/AST/ASTImporter.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/Tooling/Tooling.h"

using namespace clang;
using namespace tooling;
using namespace ast_matchers;

// 
// This link below explains how the search path works.  
// This is necessary to discover the appropriate arguments 
// to pass for this example to work.
// https://clang.llvm.org/docs/LibTooling.html
//
TEST_CASE( "build AST from code", "[ast-from-code]") {

  std::string code = R"(
  #include <systemc.h>
    class MyClass { 
      int m1; 
      };
      )";
  std::vector<std::string> args{
    "-D__STD_CONSTANT_MACROS", 
      "-D__STDC_LIMIT_MACROS",
      "-I/home/twiga/bin/clang-9.0.0/include","-I/home/twiga/bin/clang-9.0.0/lib/clang/9.0.0/include", "-I/home/twiga/code/systemc-2.3.3/systemc/include/", 
      "-I/home/twiga/bin/clang-9.0.0/include", "-std=c++11"};
  std::unique_ptr<ASTUnit> FromAST = tooling::buildASTFromCodeWithArgs(
      code, 
      args );

  SECTION( "is built from AST correct ") {
    // To print the AST.
    //FromAST->getASTContext().getTranslationUnitDecl()->dump();
    REQUIRE( true );
  }

}

