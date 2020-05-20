#ifndef _SENSITIVITY_MATCHER_H_
#define _SENSITIVITY_MATCHER_H_

#include <vector>

#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/ASTMatchers/ASTMatchersInternal.h"
#include "clang/ASTMatchers/ASTMatchersMacros.h"

using namespace clang;
using namespace clang::ast_matchers;

namespace sc_ast_matchers {

///////////////////////////////////////////////////////////////////////////////
//
// Class SensitivityMatcher
//
//
///////////////////////////////////////////////////////////////////////////////
class SensitivityMatcher : public MatchFinder::MatchCallback {
 public:
 private:
 public:
  void registerMatchers(MatchFinder &finder) {
    /* clang-format off */
    /* clang-format on */

    auto match = cxxConstructorDecl(
        forEachDescendant(
          // Find the sc_event
          cxxOperatorCallExpr(
            // Match sc_event_finder argument
            //hasDescendant(cxxMemberCallExpr(hasType(cxxRecordDecl(hasName("sc_event_finder")))
            hasArgument(1, 
              anyOf(
                hasType(cxxRecordDecl(hasName("sc_event")).bind("crd"))
              , hasType(cxxRecordDecl(hasName("sc_interface")).bind("crd"))
              , hasType(cxxRecordDecl(hasName("sc_event_finder")).bind("crd"))
              , hasType(cxxRecordDecl(isDerivedFrom(hasName("sc_port_base"))).bind("crd"))
              )
              )
            // Match clk.pos() / clk.neg()
            //, callExpr(hasDescendant()) 
            //, forEachDescendant(memberExpr().bind("member_expr")
            ).bind("cxx_operator_call_expr")
          )
        ).bind("cxx_constructor_decl");
    // Add the two matchers.
    finder.addMatcher(match, this);
  }

  // This is the callback function whenever there is a match.
  virtual void run(const MatchFinder::MatchResult &result) {
    llvm::outs()
        << " ===================== SENSITIVITY MATCHER ==================== \n";
    auto construct_decl = const_cast<CXXConstructorDecl *>(result.Nodes.getNodeAs<CXXConstructorDecl>("cxx_constructor_decl"));
    auto cxx_op_callexpr {const_cast<CXXOperatorCallExpr*>(result.Nodes.getNodeAs<CXXOperatorCallExpr>("cxx_operator_call_expr"))};
    auto decl_ref_expr{const_cast<DeclRefExpr*>(result.Nodes.getNodeAs<DeclRefExpr>("decl_ref_expr"))};
    auto crd{const_cast<CXXRecordDecl*>(result.Nodes.getNodeAs<CXXRecordDecl>("crd"))};
    auto member_expr{const_cast<MemberExpr*>(result.Nodes.getNodeAs<MemberExpr>("member_expr"))};

    if (construct_decl && cxx_op_callexpr && crd) { 
      auto name{construct_decl->getNameAsString()};
      llvm::outs() << "name: " << name << "\n";

      if (name == "test") {
        llvm::outs() << "########### FOUND TEST #######\n";
        cxx_op_callexpr->dump();
        llvm::outs()<< "#### ARGS in callexpr\n";
        auto arg_expr{cxx_op_callexpr->getArg(1)};
        if (arg_expr) {
          arg_expr->dump();
          llvm::outs() << "Trying to get type name\n";
          arg_expr->getType()->dump();

          if (member_expr) {
            llvm::outs() << " @@@@ => " << member_expr->getMemberDecl()->getNameAsString() << "\n";
            member_expr->getBase()->dump();
          }
        }

      }
    }
  }

  void dump() {}
};
};  // namespace sc_ast_matchers
#endif
