#ifndef _NETLIST_MATCHER_H_
#define _NETLIST_MATCHER_H_

#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/ASTMatchers/ASTMatchersInternal.h"
#include "clang/ASTMatchers/ASTMatchersMacros.h"

using namespace clang;
using namespace clang::ast_matchers;
using namespace scpar;

namespace sc_ast_matchers {

AST_MATCHER(CXXOperatorCallExpr, matchTest) {
  llvm::outs() << "[[OWN MATCHER]]\n";
}

///////////////////////////////////////////////////////////////////////////////
//
// Class NetlistMatcher
//
//
///////////////////////////////////////////////////////////////////////////////
class NetlistMatcher : public MatchFinder::MatchCallback {
 public:
 private:
 public:
  void registerMatchers(MatchFinder &finder) {
    /* clang-format off */
   auto match_callexpr = functionDecl(
       forEachDescendant(cxxOperatorCallExpr().bind("callexpr"))
       ).bind("functiondecl");

     //cxxOperatorCallExpr().bind("callexpr");

    /* clang-format on */

    // Add the two matchers.
    finder.addMatcher(match_callexpr, this);

  }

  // This is the callback function whenever there is a match.
  virtual void run(const MatchFinder::MatchResult &result) {
    llvm::outs() << "#### Something matcher ####\n";
    if (auto callexpr = const_cast<CXXOperatorCallExpr *>(
            result.Nodes.getNodeAs<CXXOperatorCallExpr>("callexpr"))) {
      //     std::string name{callexpr->getIdentifier()->getNameStart()};
      llvm::outs() << "#### Found CALLEXPR: \n";  // << name << "\n";
    }

    if (auto fd = const_cast<FunctionDecl *>(
            result.Nodes.getNodeAs<FunctionDecl>("functiondecl"))) {
      llvm::outs() << "#### Found FunctionDecl: \n";  // << name << "\n";
    }
  }

  void dump() {
    // Instances holds both FieldDecl and VarDecl as its base class Decl.
  }
};  // class NetlistMatcher
};  // namespace sc_ast_matchers

#endif
