#ifndef _NETLIST_MATCHER_H_
#define _NETLIST_MATCHER_H_

#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/ASTMatchers/ASTMatchersInternal.h"
#include "clang/ASTMatchers/ASTMatchersMacros.h"

#include "Matchers.h"
#include "PortBinding.h"

using namespace clang;
using namespace clang::ast_matchers;
using namespace scpar;

namespace sc_ast_matchers {
  /*

class PortBinding {
 private:
  std::string port_name_;
  MemberExpr *port_member_expr_;
  // Instance information
  std::string instance_type_;
  std::string instance_var_name_;
  std::string instance_constructor_name_;
  CXXRecordDecl *instance_type_decl_;
  Decl *instance_decl_;
  DeclRefExpr *port_dref_;

  std::string port_parameter_name_;
  DeclRefExpr *port_parameter_dref_;

 public:
  void dump() {
    llvm::outs() << "> type: " << instance_type_ << " var_name: " << instance_var_name_ << " constructor_name: " << instance_constructor_name_ << " bound to " << port_parameter_name_ << "\n";
  }

  PortBinding()
      : port_name_{},
        port_member_expr_{nullptr},
        instance_type_{},
        instance_type_decl_{},
        instance_var_name_{},
        instance_constructor_name_{},
        instance_decl_{nullptr},
        port_dref_{nullptr},
        port_parameter_name_{},
        port_parameter_dref_{} {};

  PortBinding(const std::string &port_name, MemberExpr *port_member_expr,
              const std::string &instance_type,
              const std::string &instance_var_name,
              CXXRecordDecl *instance_type_decl,
              const std::string &instance_name, Decl *instance_decl,
              DeclRefExpr *port_dref, const std::string &port_parameter_name,
              DeclRefExpr *port_parameter_dref)
      : port_name_{port_name},
        port_member_expr_{port_member_expr},
        instance_type_{instance_type},
        instance_type_decl_{instance_type_decl},
        instance_var_name_{instance_var_name},
        instance_decl_{instance_decl},
        instance_constructor_name_{instance_name},
        port_dref_{port_dref},
        port_parameter_name_{port_parameter_name},
        port_parameter_dref_{port_parameter_dref} {};
};
*/


///////////////////////////////////////////////////////////////////////////////
//
// Class NetlistMatcher
//
//
///////////////////////////////////////////////////////////////////////////////
class NetlistMatcher : public MatchFinder::MatchCallback {
 private:
  // const ModuleDeclarationMatcher::DeclarationsToInstancesMapType
  // *decl_instance_map_;
  Model *model_;
  std::string top_;

  ModuleDecl *findModuleDeclInstance(Decl *decl) {
    for (auto element : model_->getModuleInstanceMap()) {
      auto instance_list{element.second};
      auto found_inst_it =
          std::find_if(instance_list.begin(), instance_list.end(),
                       [decl](const auto &instance) {
                         return (instance->getInstanceDecl() == decl);
                       });

      if (found_inst_it != instance_list.end()) {
        return *found_inst_it;
      }
    }
    return nullptr;
  }

 public:
  void registerMatchers(MatchFinder &finder, Model *model) {
    /* clang-format off */

    model_ = model;
      
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
    std::string instance_constructor_name{};
    std::string port_param_name{};
    CXXRecordDecl *cxxdecl{nullptr};
    Decl *instance_decl{nullptr};

    if (dre_me) {
      instance_type =
          dre_me->getDecl()->getType().getBaseTypeIdentifier()->getName();
      llvm::outs() << "#### Found DeclRefExpr module type:" << instance_type
                   << "\n";

      llvm::outs() << "##### VarDecl Name: " << dre_me->getDecl()->getName()
                   << "\n";
      // TODo: Find the name of the variable from the VarDecl

      // This is the CXXRecordDecl of the instance type
      cxxdecl = dre_me->getDecl()->getType().getTypePtr()->getAsCXXRecordDecl();
      instance_decl = dre_me->getDecl();

      llvm::outs() << "#### Found DeclRefExpr CXXDecl *:" << cxxdecl << "\n";
      llvm::outs() << "#### Found DeclRefExpr Decl *:" << dre_me->getDecl()
                   << "\n";
      dre_me->getDecl()->dump();

      instance_var_name = dre_me->getFoundDecl()->getName();
      llvm::outs() << "#### Found DeclRefExpr module instance name:"
                   << instance_var_name << "\n";
    }

    if (dre) {
      port_param_name = dre->getFoundDecl()->getName();
      llvm::outs() << "#### Found DeclRefExpr parameter name: "
                   << port_param_name << "\n";
      // dre->getFoundDecl()->dump();
    }

    // Print out all the instances that were inserted in the model.
    // auto instance_map{model_->getModuleInstanceMap()};
    //
    // for (auto const &element : instance_map) {
    // auto name{element.first};
    // auto instances{element.second};
    //
    // llvm::outs() << "@@@@@@@ addr: " << name << "\n";
    // for (auto const &inst : instances) {
    // llvm::outs() << "@@@@@ instance name: " << inst->getInstanceName() << ",
    // decl*: " << inst->getInstanceDecl() << "\n";
    //
    // }
    // }
    //

    ModuleDecl *instance_module_decl{findModuleDeclInstance(instance_decl)};
    if (instance_module_decl) {
      instance_constructor_name = instance_module_decl->getInstanceName();
      llvm::outs() << "@@@@@@ instance constructor name: " << instance_constructor_name
                   << "\n";


     auto pb = new PortBinding(port_name, me,
         instance_type, instance_var_name, cxxdecl, 
         instance_constructor_name, instance_module_decl->getInstanceDecl(), 
         dre_me, port_param_name, 
         dre);
     instance_module_decl->addPortBinding(port_name, pb);
     instance_module_decl->dumpPortBinding();
    }
  }

  void dump() {
    // Instances holds both FieldDecl and VarDecl as its base class Decl.
  }
};  // class NetlistMatcher
};  // namespace sc_ast_matchers

#endif
