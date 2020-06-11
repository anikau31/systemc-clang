#ifndef _SENSITIVITY_MATCHER_H_
#define _SENSITIVITY_MATCHER_H_

#include <map>
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

    auto match_member_expr = findAll(memberExpr().bind("me"));

    auto match_member_expr_in_mcall = cxxMemberCallExpr(
        forEachDescendant(
          memberExpr().bind("me")
          ) 
        ).bind("cx");

    /* clang-format on */

    // finder.addMatcher(match_member_expr_in_mcall, this);
    finder.addMatcher(match_member_expr, this);
  }

  virtual void run(const MatchFinder::MatchResult &result) {
    auto me{const_cast<MemberExpr *>(result.Nodes.getNodeAs<MemberExpr>("me"))};
    auto cx{const_cast<CXXMemberCallExpr *>(
        result.Nodes.getNodeAs<CXXMemberCallExpr>("cx"))};

    // llvm::outs() << "==================== ME\n";
    if (me) {
      // me->dump();
      std::string name{me->getMemberDecl()->getNameAsString()};
      // llvm::outs() << "ME x: " << me->getMemberDecl()->getNameAsString()
      //             << "\n";
      calls_.insert(calls_.begin(),
                    std::make_tuple(name, me->getMemberDecl(), me));
    }
  }

  void dump() {
    for (auto const &call : calls_) {
      llvm::outs() << std::get<0>(call) << "  " << std::get<1>(call) << "  "
                   << std::get<2>(call) << "\n";
      // get<1>(call)->dump();
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
  typedef std::tuple<std::string, ValueDecl *, MemberExpr *>
      SensitivityTupleType;
  typedef std::pair<std::string, std::vector<SensitivityTupleType>>
      SensitivityPairType;

  typedef std::map<std::string, std::vector<SensitivityTupleType>> SenseMapType;

 private:
  SenseMapType sensitivity_;

  std::string generateSensitivityName(
      const std::vector<SensitivityTupleType> &sense_args) {
    std::string name{};
    for (auto const entry : sense_args) {
      name = name + std::get<0>(entry);
    }

    return name;
  }

 public:
  SenseMapType getSensitivityMap() { return sensitivity_; }

  void registerMatchers(MatchFinder &finder) {
    /* clang-format off */


    auto match = cxxConstructorDecl(isExpansionInMainFile(),
        forEachDescendant(
          // Find the sc_event
          cxxOperatorCallExpr(
            // Match sc_event_finder argument
            hasArgument(1, 
              allOf(
                anyOf(
              memberExpr(hasDeclaration(fieldDecl().bind("fd"))).bind("me") 
              ,
              cxxMemberCallExpr().bind("cxx_mcall")
              ) // anyOf
              , 
              anyOf(
                 hasType(hasUnqualifiedDesugaredType(recordType(hasDeclaration(
                    cxxRecordDecl(isSameOrDerivedFrom("sc_event")).bind("crd")))))
                , hasType(hasUnqualifiedDesugaredType(recordType(hasDeclaration(
                    cxxRecordDecl(isSameOrDerivedFrom("sc_interface")).bind("crd")))))
                , hasType(hasUnqualifiedDesugaredType(recordType(hasDeclaration(
                    cxxRecordDecl(isSameOrDerivedFrom("sc_event_finder")).bind("crd")))))
                , hasType(hasUnqualifiedDesugaredType(recordType(hasDeclaration(
                    cxxRecordDecl(isSameOrDerivedFrom("sc_port_base")).bind("crd")))))

                )//anyOf
              ) // allOf
              ) //hasArgument
          ).bind("cxx_operator_call_expr")
        )
      ).bind("cxx_constructor_decl");


    /*


    auto match = cxxConstructorDecl(isExpansionInMainFile(),
        forEachDescendant(
          // Find the sc_event
          cxxOperatorCallExpr(
            // Match sc_event_finder argument
            hasArgument(1, 
              ignoringParenCasts(
              //allOf(
                anyOf(
                    hasType(hasUnqualifiedDesugaredType(recordType(hasDeclaration(
                            cxxRecordDecl(isSameOrDerivedFrom("sc_event")).bind("crd")))))
                  , hasType(hasUnqualifiedDesugaredType(recordType(hasDeclaration(
                            cxxRecordDecl(isSameOrDerivedFrom("sc_interface")).bind("crd")))))
                  , hasType(hasUnqualifiedDesugaredType(recordType(hasDeclaration(
                            cxxRecordDecl(isSameOrDerivedFrom("sc_event_finder")).bind("crd")))))
                  , hasType(hasUnqualifiedDesugaredType(recordType(hasDeclaration(
                            cxxRecordDecl(isSameOrDerivedFrom("sc_port_base")).bind("crd")))))
                ) // anyOf
                //, 
                  //anyOf(
                    //cxxMemberCallExpr().bind("cxx_mcall")
                    //, memberExpr().bind("me")
                    //) //anyof
                //) // allOf
              ) //ignoringParenCasts
            ) // hasArgument
          ).bind("cxx_operator_call_expr")
        )
      ).bind("cxx_constructor_decl");
      */

    /* clang-format on */
    finder.addMatcher(match, this);
  }

  // This is the callback function whenever there is a match.
  virtual void run(const MatchFinder::MatchResult &result) {
    llvm::outs() << "====== SENSITIVITY MATCHER EXECUTED ======= \n";
    auto cxx_ctor_decl{const_cast<CXXConstructorDecl *>(
        result.Nodes.getNodeAs<CXXConstructorDecl>("cxx_constructor_decl"))};

    auto cxx_op_callexpr{const_cast<CXXOperatorCallExpr *>(
        result.Nodes.getNodeAs<CXXOperatorCallExpr>("cxx_operator_call_expr"))};
    auto cxx_mcall{const_cast<CXXMemberCallExpr *>(
        result.Nodes.getNodeAs<CXXMemberCallExpr>("cxx_mcall"))};
    auto me_wo_mcall{
        const_cast<MemberExpr *>(result.Nodes.getNodeAs<MemberExpr>("me"))};
    auto fd{result.Nodes.getNodeAs<FieldDecl>("fd")};
    // if (cxx_ctor_decl) {
    // cxx_ctor_decl->dump();
    //
    // }
    //

    // If the argument to the operator<<() is a MemberExpr.
    if (me_wo_mcall) {
      MatchFinder call_registry{};
      CallerCalleeMatcher call_matcher{};
      call_matcher.registerMatchers(call_registry);
      call_registry.match(*me_wo_mcall, *result.Context);
      call_matcher.dump();
      // std::vector<SensitivityTupleType>
      auto entry{call_matcher.getCallerCallee()};
      // sensitivity_.insert(std::get<0>(entry), entry);
      sensitivity_.insert(
          SensitivityPairType(generateSensitivityName(entry), entry));
    }

    // If the argument to the operator<<() is a CXXMemberCallExpr.
    // This is needed when there is a clk.pos() in the sensitivity list.
    if (cxx_mcall) {
      MatchFinder call_registry{};
      CallerCalleeMatcher call_matcher{};
      call_matcher.registerMatchers(call_registry);
      call_registry.match(*cxx_mcall, *result.Context);
      call_matcher.dump();
      // sensitivity_.insert(SensitivityPairType(call_matcher.getCallerCallee()));
      auto entry{call_matcher.getCallerCallee()};
      // sensitivity_.insert(generateSensitivityName(entry), entry);
      sensitivity_.insert(
          SensitivityPairType(generateSensitivityName(entry), entry));
    }
    llvm::outs() << "\n";
  }

  void dump() {
    for (auto const entry : sensitivity_) {
      auto generated_name{entry.first};
      auto callercallee{entry.second};
      llvm::outs() << generated_name << "  \n";

      for (auto const &call : callercallee) {
        llvm::outs() << std::get<0>(call) << "  " << std::get<1>(call) << "  "
                     << std::get<2>(call) << "\n";
      }
    }
  }
};
};  // namespace sc_ast_matchers
#endif
