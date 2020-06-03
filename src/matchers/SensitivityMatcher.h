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
/////////////////////////////////////////////////////////
//
// Class SensitivityMatcher
//
//
///////////////////////////////////////////////////////////////////////////////
class CallerCalleeMatcher : public MatchFinder::MatchCallback {
 private:
  // 1. name
  // 2. FieldDecl/VarDecl = ValueDecl. Pointer to field declaration.
  // 3. The MemberExpr from where I got this information.
  typedef std::vector<std::tuple<std::string, ValueDecl *, MemberExpr *>>
      CallerCalleeType;
  std::vector<std::tuple<std::string, ValueDecl *, MemberExpr *>> calls_;

 public:
  CallerCalleeType getCallerCallee() const { return calls_; }

  void registerMatchers(MatchFinder &finder) {
    /* clang-format off */
    auto match_member_expr = cxxMemberCallExpr(
        forEachDescendant(memberExpr().bind("me")
          ) 
        ).bind("cx");

    /* clang-format on */

    finder.addMatcher(match_member_expr, this);
  }

  virtual void run(const MatchFinder::MatchResult &result) {
    auto me{const_cast<MemberExpr *>(result.Nodes.getNodeAs<MemberExpr>("me"))};
    auto cx{const_cast<CXXMemberCallExpr *>(
        result.Nodes.getNodeAs<CXXMemberCallExpr>("cx"))};

    if (me) {
      // me->dump();
      std::string name{me->getMemberDecl()->getNameAsString()};
      llvm::outs() << "ME x: " << me->getMemberDecl()->getNameAsString()
                   << "\n";
      calls_.insert(calls_.begin(),
                    std::make_tuple(name, me->getMemberDecl(), me));
    }
  }

  void dump() {
    for (auto const &call : calls_) {
      llvm::outs() << get<0>(call) << "  " << get<1>(call) << "  "
                   << get<2>(call) << "\n";
    }
  }
};

///////////////////////////////////////////////////////////////////////////////
//
// Class SensitivityMatcher
//
//
///////////////////////////////////////////////////////////////////////////////
class SensitivityMatcher : public MatchFinder::MatchCallback {
 public:
  void registerMatchers(MatchFinder &finder) {
    /* clang-format off */
    /* clang-format on */

    auto match =
        cxxConstructorDecl(
            isExpansionInMainFile(),
            forEachDescendant(
                // Find the sc_event
                cxxOperatorCallExpr(
                    // Match sc_event_finder argument
                    hasArgument(
                        1,
                        ignoringParenCasts(allOf(
                            anyOf(hasType(hasUnqualifiedDesugaredType(
                                      recordType(hasDeclaration(
                                          cxxRecordDecl(
                                              isSameOrDerivedFrom("sc_event"))
                                              .bind("crd"))))),
                                  hasType(hasUnqualifiedDesugaredType(
                                      recordType(hasDeclaration(
                                          cxxRecordDecl(isSameOrDerivedFrom(
                                                            "sc_interface"))
                                              .bind("crd"))))),
                                  hasType(hasUnqualifiedDesugaredType(
                                      recordType(hasDeclaration(
                                          cxxRecordDecl(isSameOrDerivedFrom(
                                                            "sc_event_finder"))
                                              .bind("crd"))))),
                                  hasType(hasUnqualifiedDesugaredType(
                                      recordType(hasDeclaration(
                                          cxxRecordDecl(isSameOrDerivedFrom(
                                                            "sc_port_base"))
                                              .bind("port_crd"))))))  // anyOf
                            ,
                            // anyOf(
                            cxxMemberCallExpr(
                                // callee(
                                //  cxxMethodDecl().bind("cxx_caller")
                                //  )
                                //, hasDescendant(
                                //    memberExpr(hasDescendant(memberExpr().bind("me")))
                                //  )
                                )
                                .bind("cxx_mcall")
                            // , memberExpr().bind("me")
                            //) //anyof
                            )                 // allOf
                                           )  // ignoringParenCasts
                        )                     // hasArgument
                    )
                    .bind("cxx_operator_call_expr")))
            .bind("cxx_constructor_decl");

    finder.addMatcher(match, this);
  }

  // This is the callback function whenever there is a match.
  virtual void run(const MatchFinder::MatchResult &result) {
  //  auto cxx_construct_decl = const_cast<CXXConstructorDecl *>(
   //     result.Nodes.getNodeAs<CXXConstructorDecl>("cxx_constructor_decl"));
    auto cxx_op_callexpr{const_cast<CXXOperatorCallExpr *>(
        result.Nodes.getNodeAs<CXXOperatorCallExpr>("cxx_operator_call_expr"))};
 //   auto port_crd{const_cast<CXXRecordDecl *>(
        //result.Nodes.getNodeAs<CXXRecordDecl>("port_crd"))};
//    auto me{const_cast<MemberExpr *>(result.Nodes.getNodeAs<MemberExpr>("me"))};
    // MemberCallExpr
    auto cxx_mcall{const_cast<CXXMemberCallExpr *>(
        result.Nodes.getNodeAs<CXXMemberCallExpr>("cxx_mcall"))};
    //auto cxx_caller{const_cast<CXXMethodDecl *>(
    //    result.Nodes.getNodeAs<CXXMethodDecl>("cxx_caller"))};

    if (cxx_op_callexpr) {
      llvm::outs() << "=> x.y call expression\n";
      // cxx_op_callexpr->IgnoreImpCasts()->dump();

      if (cxx_mcall) {  // && cxx_caller) {
                        // cxx_mcall->dump();
        llvm::outs() << "=> callee: "
                     << cxx_mcall->getMethodDecl()->getNameAsString() << "\n";

        MatchFinder call_registry{};
        CallerCalleeMatcher call_matcher{};
        call_matcher.registerMatchers(call_registry);
        call_registry.match(*cxx_mcall, *result.Context);
        call_matcher.dump();
      }

    }

    // if (cxx_construct_decl && cxx_op_callexpr && port_crd) {
    // cxx_op_callexpr->dump();
    // }
  }

  void dump() {}
};
};  // namespace sc_ast_matchers
#endif
