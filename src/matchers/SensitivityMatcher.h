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
              ignoringParenCasts(
              allOf(
              anyOf(
                  hasType(hasUnqualifiedDesugaredType(recordType(hasDeclaration(
                          cxxRecordDecl(isSameOrDerivedFrom("sc_event")).bind("crd")))))
                , hasType(hasUnqualifiedDesugaredType(recordType(hasDeclaration(
                          cxxRecordDecl(isSameOrDerivedFrom("sc_interface")).bind("crd")))))
                , hasType(hasUnqualifiedDesugaredType(recordType(hasDeclaration(
                          cxxRecordDecl(isSameOrDerivedFrom("sc_event_finder")).bind("crd")))))
                , hasType(hasUnqualifiedDesugaredType(recordType(hasDeclaration(
                          cxxRecordDecl(isSameOrDerivedFrom("sc_port_base")).bind("port_crd")))))
              ) // anyOf
              , 
                anyOf(
                  cxxMemberCallExpr(
                    callee(cxxMethodDecl().bind("cxx_caller"))
                    // This is awkward. 
                    // There is a MemberExpr within a MemberExpr when we have clk.pos(). I don't know how to get access to the caller (clk) without doing the belo.w
                    // 
                    , hasDescendant(
                        memberExpr(hasDescendant(memberExpr().bind("me")))
                      )
                  ).bind("cxx_mcall")
                , memberExpr().bind("me")
                ) //anyof
              ) // allOf
            )
            )
            // Match any of the internal nodes to extract the respective fields.
            //, 
            //hasDescendant(cxxMemberCallExpr(
            //    ).bind("cxx_member_call_expr"))
            ).bind("cxx_operator_call_expr")
          )
        ).bind("cxx_constructor_decl");
    
    finder.addMatcher(match, this);
  }

  // This is the callback function whenever there is a match.
  virtual void run(const MatchFinder::MatchResult &result) {
    auto cxx_construct_decl = const_cast<CXXConstructorDecl *>(
        result.Nodes.getNodeAs<CXXConstructorDecl>("cxx_constructor_decl"));
    auto cxx_op_callexpr{const_cast<CXXOperatorCallExpr *>(
        result.Nodes.getNodeAs<CXXOperatorCallExpr>("cxx_operator_call_expr"))};
    auto port_crd{const_cast<CXXRecordDecl *>(
        result.Nodes.getNodeAs<CXXRecordDecl>("port_crd"))};
    auto me{const_cast<MemberExpr *>(result.Nodes.getNodeAs<MemberExpr>("me"))};
    // MemberCallExpr
    auto cxx_mcall{const_cast<CXXMemberCallExpr *>(
        result.Nodes.getNodeAs<CXXMemberCallExpr>("cxx_mcall"))};
    auto cxx_caller{const_cast<CXXMethodDecl *>(
        result.Nodes.getNodeAs<CXXMethodDecl>("cxx_caller"))};

    if (cxx_op_callexpr) {
      llvm::outs() << "x.y call expression\n";
      //cxx_op_callexpr->IgnoreImpCasts()->dump();

      if (cxx_mcall && cxx_caller) {
        //cxx_mcall->IgnoreImpCasts()->dump();
        // llvm::outs() << "     => callee: "
                     // << cxx_mcall->getMethodDecl()->getNameAsString() << "\n";
        llvm::outs() << "y =  " << cxx_caller->getNameAsString()
                     << "\n";
      }

      if (me) {
        //llvm::outs() << "ME\n";
        //me->IgnoreImplicit()->dump();
        llvm::outs() << "x: " << me->getMemberDecl()->getNameAsString()
                     << "\n";
        // for (auto const child : me->children()) {
          // llvm::outs() << "child\n";
          // child->dump();
        // }
      }
    }

    // if (cxx_construct_decl && cxx_op_callexpr && port_crd) {
    // cxx_op_callexpr->dump();
    // }
  }

  virtual void run2(const MatchFinder::MatchResult &result) {
    llvm::outs()
        << " ===================== SENSITIVITY MATCHER ==================== \n";
    auto construct_decl = const_cast<CXXConstructorDecl *>(
        result.Nodes.getNodeAs<CXXConstructorDecl>("cxx_constructor_decl"));
    auto cxx_op_callexpr{const_cast<CXXOperatorCallExpr *>(
        result.Nodes.getNodeAs<CXXOperatorCallExpr>("cxx_operator_call_expr"))};
    auto decl_ref_expr{const_cast<DeclRefExpr *>(
        result.Nodes.getNodeAs<DeclRefExpr>("decl_ref_expr"))};
    auto crd{const_cast<CXXRecordDecl *>(
        result.Nodes.getNodeAs<CXXRecordDecl>("crd"))};
    auto port_member_expr{const_cast<MemberExpr *>(
        result.Nodes.getNodeAs<MemberExpr>("port_member_expr"))};

    // This is to get the clk.pos()
    auto in_call_member_expr{const_cast<MemberExpr *>(
        result.Nodes.getNodeAs<MemberExpr>("in_call_member_expr"))};
    auto call_member_expr{const_cast<CXXMemberCallExpr *>(
        result.Nodes.getNodeAs<CXXMemberCallExpr>("cxx_member_call_expr"))};

    if (construct_decl && cxx_op_callexpr && crd) {
      auto name{construct_decl->getNameAsString()};
      llvm::outs() << "name: " << name << "\n";

      if (name == "test") {
        llvm::outs() << "########### FOUND TEST #######\n";
        cxx_op_callexpr->dump();
        llvm::outs() << "#### ARGS in callexpr\n";
        auto arg_expr{cxx_op_callexpr->getArg(1)};
        if (arg_expr) {
          arg_expr->IgnoreImpCasts()->dump();
          llvm::outs() << "Trying to get type name\n";
          arg_expr->getType()->dump();

          if (call_member_expr) {
            llvm::outs() << " @@@ => call member expr\n";
            call_member_expr->dump();
            llvm::outs() << "     => callee: "
                         << call_member_expr->getMethodDecl()->getNameAsString()
                         << "\n";
            llvm::outs() << "     => caller: \n";

            //        in_call_member_expr->dump();
          }
          if (port_member_expr) {
            llvm::outs() << " @@@@ => "
                         << port_member_expr->getMemberDecl()->getNameAsString()
                         << "\n";
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
