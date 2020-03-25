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

    // instance.in1(sig1);
   auto match_callexpr = functionDecl(
       forEachDescendant(
         cxxOperatorCallExpr(
               hasDescendant(
                 declRefExpr(
               hasDeclaration(varDecl()), // Match the sig1
               hasParent(implicitCastExpr()) // There must be (.) 
               ).bind("declrefexpr")
                 )
               ,
             hasDescendant(memberExpr(
               forEach(declRefExpr().bind("declrefexpr_in_memberexpr"))
               ).bind("memberexpr")
               )
           ).bind("callexpr")
         )
       ).bind("functiondecl");

     //cxxOperatorCallExpr().bind("callexpr");

    /* clang-format on */

    // Add the two matchers.
    finder.addMatcher(match_callexpr, this);
  }

  // This is the callback function whenever there is a match.
  virtual void run(const MatchFinder::MatchResult &result) {
    llvm::outs() << "#### Something matcher ####\n";
    if (auto me = const_cast<MemberExpr *>(
            result.Nodes.getNodeAs<MemberExpr>("memberexpr"))) {
      std::string portName { me->getMemberDecl()->getNameAsString() };
      llvm::outs() << "#### Found MemberExpr portname: " << portName << "\n";
    }

    if (auto ie = const_cast<ImplicitCastExpr*>(
            result.Nodes.getNodeAs<ImplicitCastExpr>("impcastexpr"))) {
      llvm::outs() << "#### Found ImplicitCastExpr: \n";// << portName << "\n";
      ie->dump();
    }

    if (auto dre = const_cast<DeclRefExpr *>(
            result.Nodes.getNodeAs<DeclRefExpr>("declrefexpr_in_memberexpr"))) {
      std::string name{
          dre->getDecl()->getType().getBaseTypeIdentifier()->getName()};
      llvm::outs() << "#### Found DeclRefExpr module type:" << name << "\n";
    }


    if (auto dre = const_cast<DeclRefExpr *>(
            result.Nodes.getNodeAs<DeclRefExpr>("declrefexpr"))) {
      std::string name{
          dre->getFoundDecl()->getName()};
      llvm::outs() << "#### Found DeclRefExpr parameter name: " << name << "\n";
      dre->getFoundDecl()->dump();
    }
  }

  void dump() {
    // Instances holds both FieldDecl and VarDecl as its base class Decl.
  }
};  // class NetlistMatcher
};  // namespace sc_ast_matchers

#endif
