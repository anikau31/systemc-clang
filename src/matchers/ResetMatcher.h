#ifndef _RESET_MATCHER_H_
#define _RESET_MATCHER_H_

#include "clang/ASTMatchers/ASTMatchers.h"
#include "llvm/Support/Debug.h"

/// Different matchers may use different DEBUG_TYPE
#undef DEBUG_TYPE
#define DEBUG_TYPE "ResetMatcher"

using namespace clang::ast_matchers;

namespace sc_ast_matchers {
using namespace clang;

class ResetMatcher : public MatchFinder::MatchCallback {
 private:
  std::pair<std::string, const clang::Expr*> reset_signal_;
  std::pair<std::string, const clang::Expr*> edge_;
  bool reset_async_;

 public:
  std::pair<std::string, const clang::Expr*> getResetSignal() const {
    return reset_signal_;
  }
  std::pair<std::string, const clang::Expr*> getResetEdge() const {
    return edge_;
  }
  bool getResetType() const { return reset_async_; }

 public:
  void registerMatchers(MatchFinder& finder, clang::MemberExpr* process_expr) {
    /// Get type of process_expr.
    std::string member_name{process_expr->getMemberDecl()->getNameAsString()};

    // clang-format off
    auto reset_matcher = stmt(
        anyOf(
          allOf(
            hasDescendant(
              memberExpr(hasDeclaration(cxxMethodDecl(hasName(member_name)))).bind("cxx_me_proc")
            )
            ,
            hasDescendant(
              cxxMemberCallExpr(callee(cxxMethodDecl(hasName("async_reset_signal_is")))).bind("cxx_me_async")
              ) // hasDescendant
            ) //allOf
        ,

          allOf(
            hasDescendant(
              memberExpr(hasDeclaration(cxxMethodDecl(hasName(member_name)))).bind("cxx_me_proc")
            )
            ,
            hasDescendant(
              cxxMemberCallExpr(callee(cxxMethodDecl(hasName("reset_signal_is")))).bind("cxx_me_sync")
              ) // hasDescendant
          ) //allOf
        ) //anyOf
      ).bind("reset_expr");
    // clang-format on

    finder.addMatcher(reset_matcher, this);
  }

  virtual void run(const MatchFinder::MatchResult& result) {
    llvm::dbgs() << "RESET MATCHER FOUND\n";
    // auto ctor_decl{
    // result.Nodes.getNodeAs<clang::CXXConstructorDecl>("reset_expr")};
    auto cxx_me_proc{result.Nodes.getNodeAs<clang::MemberExpr>("cxx_me_proc")};
    auto cxx_me_async{
        result.Nodes.getNodeAs<clang::CXXMemberCallExpr>("cxx_me_async")};
    auto cxx_me_sync{
        result.Nodes.getNodeAs<clang::CXXMemberCallExpr>("cxx_me_sync")};
    const clang::CXXMemberCallExpr* cxx_me{nullptr};

    // if (cxx_me_proc) {
      // llvm::dbgs() << " PROC MATCH\n";
      // cxx_me_proc->dump();
    // }
//
    cxx_me = cxx_me_sync;

    reset_async_ = false;
    if (cxx_me_async) {
      reset_async_ = true;
      llvm::dbgs() << "SETTING RESET TO TRUE\n";
      cxx_me = cxx_me_async;
    }

    if (cxx_me) {
      cxx_me->dump();
      /// Two arguments.
      const clang::Expr* rsig{cxx_me->getArg(0)->IgnoreImplicit()};
      const clang::Expr* high_low{cxx_me->getArg(1)->IgnoreImplicit()};

      if (rsig && high_low) {
        const clang::MemberExpr* rsig_name =
            clang::dyn_cast<clang::MemberExpr>(rsig);

        llvm::dbgs() << "high low \n";
        high_low->dump();

        const clang::CXXBoolLiteralExpr* edge_name =
            clang::dyn_cast<clang::CXXBoolLiteralExpr>(high_low);

        if (rsig_name && edge_name) {
          reset_signal_.first = rsig_name->getMemberDecl()->getNameAsString();
          reset_signal_.second = rsig;

          edge_.first = "false";
          if (edge_name->getValue()) {
            edge_.first = "true";
          }
          edge_.second = high_low;
        }
      }
    }
  }

  void dump() {
    llvm::dbgs() << "reset_signal " << reset_signal_.first << "\n";
    llvm::dbgs() << "active_edge  " << edge_.first << "\n";
    llvm::dbgs() << "reset_async " << reset_async_ << "\n";
  }
};
};  // namespace sc_ast_matchers

#endif
