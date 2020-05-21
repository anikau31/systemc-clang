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

            // Match any of the internal nodes to extract the respective fields.
            , 
            hasDescendant(cxxMemberCallExpr(
                //on(declRefExpr().bind("on_decl_ref_expr"))
                //,
                //hasDescendant(memberExpr().bind("in_call_member_expr"))
                ).bind("cxx_member_call_expr"))
            // Identifies sc_port_base 
            //hasDescendant(memberExpr(hasDeclaration(fieldDecl(hasType(cxxRecordDecl(isDerivedFrom(hasName("sc_port_base"))))))).bind("port_member_expr"))
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
    auto port_member_expr{const_cast<MemberExpr*>(result.Nodes.getNodeAs<MemberExpr>("port_member_expr"))};

    // This is to get the clk.pos()
    auto in_call_member_expr{const_cast<MemberExpr*>(result.Nodes.getNodeAs<MemberExpr>("in_call_member_expr"))};
    auto call_member_expr{const_cast<CXXMemberCallExpr*>(result.Nodes.getNodeAs<CXXMemberCallExpr>("cxx_member_call_expr"))};

    if (construct_decl && cxx_op_callexpr && crd) { 
      auto name{construct_decl->getNameAsString()};
      llvm::outs() << "name: " << name << "\n";

      if (name == "test") {
        llvm::outs() << "########### FOUND TEST #######\n";
        cxx_op_callexpr->dump();
        llvm::outs()<< "#### ARGS in callexpr\n";
        auto arg_expr{cxx_op_callexpr->getArg(1)};
        if (arg_expr) {
          arg_expr->IgnoreImpCasts()->dump();
          llvm::outs() << "Trying to get type name\n";
          arg_expr->getType()->dump();

          if (call_member_expr) {
            llvm::outs() << " @@@ => call member expr\n";
            call_member_expr->dump();
            llvm::outs() << "     => callee: " << call_member_expr->getMethodDecl()->getNameAsString() << "\n";
            llvm::outs() << "     => caller: \n";

    //        in_call_member_expr->dump();

          }
          if (port_member_expr) {
            llvm::outs() << " @@@@ => " << port_member_expr->getMemberDecl()->getNameAsString() << "\n";
            port_member_expr->getBase()->dump();
          }
        }

      }
    }
  }

  void dump() {}
};
};  // namespace sc_ast_matchers
#endif
