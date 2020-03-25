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

class PortBinding {
 private:
  std::string port_name_;
  MemberExpr *port_member_expr_;
  // Instance information
  std::string instance_type_;
  CXXRecordDecl *instance_type_decl_;
  std::string instance_var_name_;
  Decl *instance_decl_;
  DeclRefExpr *port_dref_;

  std::string port_parameter_name_;
  DeclRefExpr *port_parameter_dref_;

 public:
  PortBinding()
      : port_name_{},
        port_member_expr_{nullptr},
        instance_type_{},
        instance_type_decl_{},
        instance_var_name_{},
        instance_decl_{nullptr},
        port_dref_{nullptr},
        port_parameter_name_{},
        port_parameter_dref_{} {};

  PortBinding(const std::string &port_name, MemberExpr *port_member_expr,
              const std::string &instance_type, CXXRecordDecl *instance_type_decl,
              const std::string &instance_name, Decl *instance_decl,
              DeclRefExpr *port_dref, const std::string &port_parameter_name,
              DeclRefExpr *port_parameter_dref)
      : port_name_{port_name},
        port_member_expr_{port_member_expr},
        instance_type_{instance_type},
        instance_type_decl_{instance_type_decl},
        instance_var_name_{instance_name},
        instance_decl_{instance_decl},
        port_dref_{port_dref},
        port_parameter_name_{port_parameter_name},
        port_parameter_dref_{port_parameter_dref} {};
};
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
               hasDeclaration(varDecl().bind("bound_variable")), // Match the sig1
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
  //
  // To save:
  // 1. port_name
  // 2. port_parameter
  // 3. Instance variable name
  // 4. Instance variable type
  //
  virtual void run(const MatchFinder::MatchResult &result) {
    llvm::outs() << "#### Something matcher ####\n";
    auto me{const_cast<MemberExpr *>(
        result.Nodes.getNodeAs<MemberExpr>("memberexpr"))};
    auto ie{const_cast<ImplicitCastExpr *>(
        result.Nodes.getNodeAs<ImplicitCastExpr>("impcastexpr"))};
    auto dre_me{const_cast<DeclRefExpr *>(
        result.Nodes.getNodeAs<DeclRefExpr>("declrefexpr_in_memberexpr"))};
    auto dre{const_cast<DeclRefExpr *>(
        result.Nodes.getNodeAs<DeclRefExpr>("declrefexpr"))};

    std::string port_name{};
    if (me) {
      port_name = me->getMemberDecl()->getNameAsString();
      llvm::outs() << "#### Found MemberExpr portname: " << port_name << "\n";
    }

    if (ie) {
      llvm::outs()
          << "#### Found ImplicitCastExpr: \n";  // << portName << "\n";
      ie->dump();
    }

    std::string instance_type{};
    std::string instance_var_name{};
    std::string port_param_name{};
    CXXRecordDecl *cxxdecl{nullptr};

    if (dre_me) {
      instance_type =
          dre_me->getDecl()->getType().getBaseTypeIdentifier()->getName();
      llvm::outs() << "#### Found DeclRefExpr module type:" << instance_type
                   << "\n";
      llvm::outs() << "#### Found DeclRefExpr Decl *:" << dre_me->getDecl()
                   << "\n";
      dre_me->getDecl()->dump();

      //TODo: Find the name of the variable from the VarDecl
      llvm::outs() << "##### VarDecl Name: " << dre_me->getDecl()->getName() << "\n";
     

      // This is the CXXRecordDecl of the instance type
      cxxdecl = dre_me->getDecl()->getType().getTypePtr()->getAsCXXRecordDecl();
      llvm::outs() << "#### Found DeclRefExpr CXXDecl *:" << cxxdecl << "\n";

      instance_var_name = dre_me->getFoundDecl()->getName();
      llvm::outs() << "#### Found DeclRefExpr module instance name:"
                   << instance_var_name << "\n";
    }

    if (dre) {
      port_param_name = dre->getFoundDecl()->getName();
      llvm::outs() << "#### Found DeclRefExpr parameter name: "
                   << port_param_name << "\n";
      //dre->getFoundDecl()->dump();
    }

    // new PortBinding(port_name, me, instance_type, instance_var_name,
    // cxxdecl, dre_me->getDecl(), dre_me, port_param_name, dre);
  }

  void dump() {
    // Instances holds both FieldDecl and VarDecl as its base class Decl.
  }
};  // class NetlistMatcher
};  // namespace sc_ast_matchers

#endif
