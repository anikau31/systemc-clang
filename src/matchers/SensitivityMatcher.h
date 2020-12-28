//===- SensitivityMatcher.h - Matching sensitivity lists --*- C++ -*-=====//
//
// Part of the systemc-clang project.
// See License.rst
//
//===----------------------------------------------------------------------===//
//
/// \file
/// Parses a SystemC module's process' sensitivity list.
//
/// \author Hiren Patel
//===----------------------------------------------------------------------===//

#ifndef _SENSITIVITY_MATCHER_H_
#define _SENSITIVITY_MATCHER_H_

#include <map>
#include <vector>

// Clang includes.
//
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "llvm/Support/Debug.h"

#include "ArrayTypeUtils.h"
/// Different matchers may use different DEBUG_TYPE
#undef DEBUG_TYPE
#define DEBUG_TYPE "SensitivityMatcher"

namespace sc_ast_matchers {
using namespace clang::ast_matchers;
using namespace sc_ast_matchers::utils::array_type;
///////////////////////////////////////////////////////////////////////////////
///
/// Class CallerCaleeMatcher
///
/// This class identifies for a method call, who is the caller, and who is the
/// callee.
///
///////////////////////////////////////////////////////////////////////////////
class CallerCalleeMatcher : public MatchFinder::MatchCallback {
 private:
  /// 1. name
  /// 2. FieldDecl/VarDecl = ValueDecl. Pointer to field declaration.
  /// 3. The MemberExpr from where I got this information.
  typedef std::vector<
      std::tuple<std::string, clang::ValueDecl *, clang::MemberExpr *,
                 clang::DeclRefExpr *, clang::ArraySubscriptExpr *>>
      CallerCalleeType;

  /// Store the information.
  std::vector<std::tuple<std::string, clang::ValueDecl *, clang::MemberExpr *,
                         clang::DeclRefExpr *, clang::ArraySubscriptExpr *>>
      calls_;

  clang::DeclRefExpr *vd_;

 public:
  /// This returns a list of all the caller and callees that are identified.
  CallerCalleeType getCallerCallee() const { return calls_; }

  /// Register the matchers to identify caller and callees.
  void registerMatchers(MatchFinder &finder, clang::DeclRefExpr *vd) {
    vd_ = vd;

    /* clang-format off */

    /// This is the matcher that finds all the member expressions..  
    auto match_member_expr = findAll(memberExpr().bind("me"));

    /* clang-format on */
    finder.addMatcher(match_member_expr, this);
  }

  /// This is the callback function when there is a match.
  virtual void run(const MatchFinder::MatchResult &result) {
    auto me{const_cast<clang::MemberExpr *>(
        result.Nodes.getNodeAs<clang::MemberExpr>("me"))};

    if (me) {
      std::string name{me->getMemberDecl()->getNameAsString()};
      calls_.insert(calls_.begin(), std::make_tuple(name, me->getMemberDecl(),
                                                    me, vd_, nullptr));
    }
  }

  /// Dump out the caller and callee found in the sensitivity list.
  void dump() {
    for (auto const &call : calls_) {
      (llvm::outs() << std::get<0>(call) << "  " << std::get<1>(call) << "  "
                    << std::get<2>(call) << "\n");
    }
  }
};

////////////////////////////////////////////////////////////////////////////
///
/// class SensitiveOperatorCallMatcher
///
////////////////////////////////////////////////////////////////////////////
class SensitiveOperatorCallMatcher : public MatchFinder::MatchCallback {
 private:
  clang::CXXMemberCallExpr *cxx_mcall_;
  clang::MemberExpr *me_wo_mcall_;
  const clang::ArraySubscriptExpr *array_fd_;

 public:
  clang::CXXMemberCallExpr *getMemberExprCallExpr() { return cxx_mcall_; }

  clang::MemberExpr *getMemberExprWithoutCall() { return me_wo_mcall_; }
  const clang::ArraySubscriptExpr *getMemberArraySubscriptExpr() {
    return array_fd_;
  }

  /// This is the main matcher for identifying sensitivity lists.
  ///
  /// The matcher starts in the constructor of a SystemC module.
  /// 1. Within the constructor, we need to identify operator<< since that is
  /// the overloaded member used to add sensitivity parameters.
  ///
  /// 2. We want to find every operator<< call with an argument, and those
  /// that have the FieldDecl used to be of a certain type (sc_event,
  /// sc_interface, ...). This is done because those are the parameters that
  /// are allowed for the operator<<.
  ///
  /// 2a. For the argument, we identify a call expression (cxxMemberCallExpr),
  /// or just a MemberExpr that has a declaration of a FieldDecl.
  /// 2b. The parameters that are allowed in operator<< are the following.
  ///  - sc_event
  ///  - sc_interface
  ///  - sc_event_finder
  ///  - sc_port_base
  ///

  void registerMatchers(MatchFinder &finder) {
    /* clang-format off */

    auto match_opcall= 
          cxxOperatorCallExpr(
            // Match sc_event_finder argument
            hasArgument(1, 
              allOf(
                anyOf(
                  memberExpr(
                    hasDeclaration(
                      fieldDecl().bind("fd")
                      ) //hasDeclaration
                    ).bind("me") 
                  ,
                  cxxMemberCallExpr().bind("cxx_mcall")
                  , 
                  arraySubscriptExpr().bind("array_fd")
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
          ).bind("cxx_operator_call_expr");

    ///  clang-format on

    finder.addMatcher(match_opcall, this);
  }

  virtual void run(const MatchFinder::MatchResult &result) {
    LLVM_DEBUG(llvm::dbgs() << "#### OperatorCallMatcher\n");

    auto array_fd{result.Nodes.getNodeAs<clang::ArraySubscriptExpr>("array_fd")};

    if (array_fd) {
      llvm::outs() << " @@@@@@@@@@@@@@@@@@@@@@@@ ARRAY FD @@@@@@@@@@@@@@@@@@@@@@@\n";
      array_fd->dump();
      array_fd_ = array_fd;
    }

    auto cxx_mcall{const_cast<clang::CXXMemberCallExpr*>(
        result.Nodes.getNodeAs<clang::CXXMemberCallExpr>("cxx_mcall"))};
    auto me_wo_mcall{
        const_cast<clang::MemberExpr*>(result.Nodes.getNodeAs<clang::MemberExpr>("me"))};

    if (cxx_mcall) { cxx_mcall_ = cxx_mcall; }
    if (me_wo_mcall) { me_wo_mcall_ = me_wo_mcall; }
  }
};



///////////////////////////////////////////////////////////////////////////////
//
/// Class SensitivityMatcher
//
//
///////////////////////////////////////////////////////////////////////////////
class SensitivityMatcher : public MatchFinder::MatchCallback {
 public:
  /// A sensitivity will typically have a member field, and sometimes a call on
  /// the member field. The ValueDecl records the FieldDecl, and the VarDecl
  /// that may be used in the sensitivity list. The MemberExpr is just a way to
  /// represent where this particular ValueDecl was found for further parsing.
  /// The VarDecl* is the process handle to which this sensitivity is bound to.

  typedef std::tuple<std::string, clang::ValueDecl*, clang::MemberExpr*, clang::DeclRefExpr*, clang::ArraySubscriptExpr*> 
      SensitivityTupleType;

  /// This is the pair for inserting key-value entries in the map.
  typedef std::pair<std::string, std::vector<SensitivityTupleType>>
      SensitivityPairType;

  /// The key is going to be the name of the FieldDecl/VarDecl.
  //typedef std::map<std::string, std::vector<SensitivityTupleType>> SenseMapType;
  typedef std::map<std::string, std::vector<SensitivityTupleType>> SenseMapType;

 private:
  /// This is the map structure to store the identified sensitivity list.
  SenseMapType sensitivity_;

  /// This provides access to the SystemC process' entry function handler. 
  clang::DeclRefExpr *process_handle_;

  /// This generates an encoded name of the argument for the sensitivity.
  std::string generateSensitivityName(
      const std::vector<SensitivityTupleType> &sense_args) {
    std::string name{};
    std::string ef_name{};
    for (auto const entry : sense_args) {
      name = name + std::get<0>(entry);
      auto vd{std::get<3>(entry)};
      if (vd) { ef_name = vd->getNameInfo().getAsString();}
    }
    return ef_name+"__"+name;
  }

 public:
  /// \brief  Return the sensitivity map that has been created.
  /// \return A map of sensitivity lists found.
  ///
  /// This method just returns the identified map of sensitivity lists.
  SenseMapType getSensitivityMap() { return sensitivity_; }

  /// \brief Defines the matcher, and setup the matcher.
  ///
  void registerMatchers(MatchFinder &finder) {
  /* clang-format off */

    auto match = 
      cxxConstructorDecl(
        has(
          compoundStmt(
            forEachDescendant(
              cxxOperatorCallExpr(optionally(
                  hasDescendant(declRefExpr(hasType(cxxRecordDecl(hasName("::sc_core::sc_process_handle")))).bind("process_handle"))
                  )
                ).bind("opcall")
            ) 
          ).bind("compound_stmt")
        )
      ).bind("cxx_constructor_decl");

    /* clang-format on */
    /// Add the matcher.
    finder.addMatcher(match, this);
  }

  /// This is the callback function whenever there is a match.
  virtual void run(const MatchFinder::MatchResult &result) {
    // LLVM_DEBUG(llvm::dbgs()
    DEBUG_WITH_TYPE("matcher-header",
                    llvm::dbgs()
                        << "====== SENSITIVITY MATCHER EXECUTED ======= \n");

    clang::CXXMemberCallExpr *cxx_mcall{};
    clang::MemberExpr *me_wo_mcall{};
    clang::ArraySubscriptExpr *array_expr{};

    /// Debug code
    auto process_handle{const_cast<clang::DeclRefExpr *>(
        result.Nodes.getNodeAs<clang::DeclRefExpr>("process_handle"))};
    auto opcall{const_cast<clang::CXXOperatorCallExpr *>(
        result.Nodes.getNodeAs<clang::CXXOperatorCallExpr>("opcall"))};

    if (opcall) {
      opcall->dump();
      // Check if there is process handle
      if (process_handle) {
        clang::ValueDecl *vd{process_handle->getDecl()};
        LLVM_DEBUG(llvm::dbgs() << "# DeclRefExpr "
                                << process_handle->getNameInfo().getAsString()
                                << "   => " << vd->getNameAsString() << "\n");
        process_handle_ = process_handle;
      }

      MatchFinder sense_registry{};
      SensitiveOperatorCallMatcher sop_matcher{};
      sop_matcher.registerMatchers(sense_registry);
      sense_registry.match(*opcall, *result.Context);

      cxx_mcall = sop_matcher.getMemberExprCallExpr();
      me_wo_mcall = sop_matcher.getMemberExprWithoutCall();
      array_expr = const_cast<clang::ArraySubscriptExpr *>(
          sop_matcher.getMemberArraySubscriptExpr());
    }

    /// If the argument to the operator<<() is a MemberExpr.
    /// This is the situation when we only have a FieldDecl as a part of the
    /// sensitivity list. This is when there is no member call being invoked on
    /// the FieldDecl.

    if (process_handle_) {
      if (me_wo_mcall) {
        MatchFinder call_registry{};
        CallerCalleeMatcher call_matcher{};
        call_matcher.registerMatchers(call_registry, process_handle_);
        call_registry.match(*me_wo_mcall, *result.Context);

        LLVM_DEBUG(call_matcher.dump());
        auto entry{call_matcher.getCallerCallee()};
        sensitivity_.insert(
            SensitivityPairType(generateSensitivityName(entry), entry));
      }

      /// If the argument to the operator<<() is a CXXMemberCallExpr.
      /// This is needed when there is a clk.pos() in the sensitivity list.
      /// This is when we have member call expressions on the FieldDecl.
      if (cxx_mcall) {
        MatchFinder call_registry{};
        CallerCalleeMatcher call_matcher{};
        call_matcher.registerMatchers(call_registry, process_handle_);
        call_registry.match(*cxx_mcall, *result.Context);
        LLVM_DEBUG(call_matcher.dump());

        auto entry{call_matcher.getCallerCallee()};
        sensitivity_.insert(
            SensitivityPairType(generateSensitivityName(entry), entry));
      }

      /// If the argument is an ArraySubscriptExpr, then only provide access to
      /// the pointer to ArraySubscriptExpr.
      if (array_expr) {
        llvm::outs() << "@@@@ Parse the array \n";
        auto me{getArrayMemberExprName(array_expr)};
        std::string name{me->getMemberDecl()->getNameAsString()};
        auto entry{std::make_tuple(name, me->getMemberDecl(),
                                   const_cast<clang::MemberExpr *>(me),
                                   process_handle_, array_expr)};
        std::vector<
            std::tuple<std::string, clang::ValueDecl *, clang::MemberExpr *,
                       clang::DeclRefExpr *, clang::ArraySubscriptExpr *>>
            calls;
        calls.insert(calls.begin(),
                     std::make_tuple(name, me->getMemberDecl(),
                                     const_cast<clang::MemberExpr *>(me),
                                     process_handle_, array_expr));

        sensitivity_.insert(
            SensitivityPairType(generateSensitivityName(calls), calls));
      }
    }
    LLVM_DEBUG(dump());
  }

  /// \brief Dump out the detected sensitivity list for every process.
  ///
  /// This is going to print out the sensitivity list.
  void dump() {
    for (auto const entry : sensitivity_) {
      auto generated_name{entry.first};
      auto callercallee{entry.second};
      LLVM_DEBUG(llvm::dbgs() << generated_name << "  \n");

      for (auto const &call : callercallee) {
        LLVM_DEBUG(llvm::dbgs()
                   << std::get<0>(call) << "  " << std::get<1>(call) << "  "
                   << std::get<2>(call) << "\n");

        if (auto array_expr = std::get<4>(call)) {
          llvm::dbgs() << "ArraySubscriptExpr\n";
          array_expr->dump();
        }
      }
    }
  }
};
};  // namespace sc_ast_matchers
#endif
