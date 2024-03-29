#include <doctest.h>
#include "SystemCClang.h"

// This is automatically generated from cmake.
#include "ClangArgs.h"
#include "Testing.h"

#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/ASTMatchers/ASTMatchersInternal.h"
#include "clang/ASTMatchers/ASTMatchersMacros.h"
#include "CallExprUtils.h"

using namespace clang::ast_matchers;
using namespace systemc_clang;
using namespace sc_ast_matchers;
using namespace sc_ast_matchers::utils;

#undef DEBUG_TYPE
#define DEBUG_TYPE "Tests"

class CallsMatcher : public MatchFinder::MatchCallback {
 public:
  void registerMatchers(MatchFinder &finder) {
    /* clang-format off */
    auto cxx_decl = cxxRecordDecl(
      has(functionDecl(forEachDescendant(stmt().bind("stmt")), 
      hasName("simple_wait")).bind("fdecl")),
                  isExpansionInMainFile()).bind("cxx_decl");
    //auto stmt_expr = stmt().bind("stmt");
    /* clang-format on */

    auto sc_min =
        cxxRecordDecl(has(functionDecl(forEachDescendant(stmt().bind("stmt")),
                                       hasName("simple_wait"))
                              .bind("fdecl")),
                      isExpansionInMainFile())
            .bind("cxx_decl");

    // finder.addMatcher(stmt_expr, this);
    finder.addMatcher(cxx_decl, this);
  }

  virtual void run(const MatchFinder::MatchResult &result) {
    auto fdecl{result.Nodes.getNodeAs<FunctionDecl>("fdecl")};
    auto cdecl{result.Nodes.getNodeAs<CXXRecordDecl>("cxx_decl")};
    auto stmt{result.Nodes.getNodeAs<Stmt>("stmt")};

    auto call_expr{result.Nodes.getNodeAs<CallExpr>("call_expr")};
    LLVM_DEBUG(llvm::outs() << "=============== TEST SC CALLS====== \n";);

    if (cdecl && fdecl && stmt) {
      llvm::dbgs() << "cxx decl " << cdecl->getNameAsString() << "\n";
      //cdecl->dump();
      // checkWaitCalls(stmt, result);
      checkSCFunctions(stmt, result);
    }
  }

  void checkSCFunctions(const Stmt *stmt,
                        const MatchFinder::MatchResult &result) {
    auto st = stmt;
    // st->dump();


    // Checks sc_min, sc_max, sc_abs
    if (auto cexpr = dyn_cast<CallExpr>(st)) {
      llvm::dbgs() << "\n1. ########## CallExpr \n";
      cexpr->dump();
      std::vector<llvm::StringRef> scdt{"sc_dt", "sc_core"};
      if (isInNamespace(cexpr, *result.Context, scdt)) {
        llvm::dbgs() << "#### SC FUNCTION FOUND called ";
        dumpExprName(cexpr);
        llvm::dbgs() << " ";
        clang::LangOptions lopt;
        SourceRange sr = cexpr->getSourceRange();
        sr.dump(*result.SourceManager);
      }
    }
  }

  void checkWaitCalls(const Stmt *stmt,
                      const MatchFinder::MatchResult &result) {
    auto st = stmt;
    st->dump();

    if (auto expr = dyn_cast<Expr>(st)) {
      llvm::dbgs() << "\n########## ISExpr \n";
      expr->dump();
      std::vector<llvm::StringRef> wait{"sc_core"};
      if (isInNamespace(expr, *result.Context, wait)) {
        llvm::dbgs() << "#### WAIT FOUND at ";
        dumpExprName(expr);
        clang::LangOptions lopt;
        SourceRange sr = expr->getSourceRange();
        sr.dump(*result.SourceManager);
      }
      llvm::dbgs() << "\n";
    }
  }
  void dump() {}
};

TEST_CASE("Read SystemC model from file for testing") {
  std::string code{systemc_clang::read_systemc_file(
      systemc_clang::test_data_dir, "sc-calls-input.cpp")};

  ASTUnit *from_ast =
      tooling::buildASTFromCodeWithArgs(code, systemc_clang::catch_test_args)
          .release();

  CallsMatcher call_match{};
  MatchFinder match_reg{};
  call_match.registerMatchers(match_reg);
  // Run all the matchers
  match_reg.matchAST(from_ast->getASTContext());
}
