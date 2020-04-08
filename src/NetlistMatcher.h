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
  ModuleDeclarationMatcher *module_matcher_;
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
    void registerMatchers(MatchFinder &finder, Model *model,
                        ModuleDeclarationMatcher *module_matcher) {
    /* clang-format off */

    model_ = model;
    module_matcher_ = module_matcher;
      
    // instance.in1(sig1);
   auto match_sc_main_callexpr = functionDecl(
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

    /* clang-format on */

    auto match_ctor_decl =
        namedDecl(
            has(compoundStmt(
                    forEachDescendant(
                        cxxOperatorCallExpr(
                            // Port
                            // 1. MemberExpr has a descendant that is a
                            // MemberExpr The first MemberExpr gives the port
                            // bound to information. The second MemberExpr gives
                            // the instance information to whom the port
                            // belongs.
                            hasDescendant(
                                memberExpr(has(memberExpr().bind(
                                               "memberexpr_instance")))
                                    .bind("memberexpr_port")),
                            // Arguments
                            // 1. Has MemberExpr as a descendant.
                            //   1.a It must have an implicitCastExpr as a
                            //   parent 1.b But not a MemberExpr (This is
                            //   because it matches with the port others.
                            hasDescendant(
                                memberExpr(hasParent(implicitCastExpr()),
                                           unless(hasDescendant(memberExpr())))
                                    .bind("memberexpr_arg")))
                            .bind("callexpr")))
                    .bind("compoundstmt")))
            .bind("functiondecl");

    // Add the two matchers.
    finder.addMatcher(match_sc_main_callexpr, this);
    finder.addMatcher(match_ctor_decl, this);
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
    bool is_ctor_binding{true};

    auto me{const_cast<MemberExpr *>(
        result.Nodes.getNodeAs<MemberExpr>("memberexpr"))};
    auto dre_me{const_cast<DeclRefExpr *>(
        result.Nodes.getNodeAs<DeclRefExpr>("declrefexpr_in_memberexpr"))};
    auto dre{const_cast<DeclRefExpr *>(
        result.Nodes.getNodeAs<DeclRefExpr>("declrefexpr"))};
    auto fd{const_cast<FunctionDecl *>(
        result.Nodes.getNodeAs<FunctionDecl>("functiondecl"))};

    //////////////////////////////////////////////////////
    /// TESTING
    auto mexpr_port{const_cast<MemberExpr *>(
        result.Nodes.getNodeAs<MemberExpr>("memberexpr_port"))};
    auto mexpr_instance{const_cast<MemberExpr *>(
        result.Nodes.getNodeAs<MemberExpr>("memberexpr_instance"))};
    auto mexpr_arg{const_cast<MemberExpr *>(
        result.Nodes.getNodeAs<MemberExpr>("memberexpr_arg"))};

    if (mexpr_port && mexpr_instance && mexpr_arg) {
      is_ctor_binding = true;
      llvm::outs() << "#### Found MemberExpr: "
                   << "\n";
      auto port = mexpr_port->getMemberNameInfo().getAsString();
      auto port_type{mexpr_port->getMemberDecl()->getType().getTypePtr()};
      auto port_type_name = mexpr_port->getMemberDecl()
                                ->getType()
                                .getBaseTypeIdentifier()
                                ->getName();
      // mexpr_port->dump();

      auto instance_var_name =
          mexpr_instance->getMemberNameInfo().getAsString();
      auto instance_type{
          mexpr_instance->getMemberDecl()->getType().getTypePtr()};
      auto instance_type_name = mexpr_instance->getMemberDecl()
                                    ->getType()
                                    .getBaseTypeIdentifier()
                                    ->getName();
      // mexpr_instance->dump();

      auto port_arg = mexpr_arg->getMemberNameInfo().getAsString();
      auto port_arg_type{mexpr_arg->getMemberDecl()->getType().getTypePtr()};
      auto port_arg_type_name = mexpr_arg->getMemberDecl()
                                    ->getType()
                                    .getBaseTypeIdentifier()
                                    ->getName();
    }

    //////////////////////////////////////////////////////
    /// TESTING
    //

    std::string port_name{};
    if (me && dre_me && dre) {
      is_ctor_binding = false;
      port_name = me->getMemberDecl()->getNameAsString();
    }

    Decl *instance_decl{nullptr};
    if (is_ctor_binding) {
      if (mexpr_instance) {
        instance_decl = mexpr_instance->getMemberDecl();
      }
    } else {
      if (dre_me) {
        instance_decl =
            dre_me->getDecl()->getType().getTypePtr()->getAsCXXRecordDecl();
        // instance_decl = dre_me->getDecl();
      }
    }
    // instance_decl->dump();

    ModuleDecl *instance_module_decl{findModuleDeclInstance(instance_decl)};
    if (!instance_module_decl) {
      llvm::outs() << "@@@@@ No instance module decl found \n";
    }

    if (instance_module_decl) {
      auto instance_constructor_name = instance_module_decl->getInstanceName();
      llvm::outs() << "@@@@@@ instance constructor name: "
                   << instance_constructor_name << "\n";

      PortBinding *pb{nullptr};

      if (is_ctor_binding) {
        pb = new PortBinding(mexpr_port, mexpr_instance, mexpr_arg);
      } else {
        llvm::outs() << "=> found instance in sc_main\n";
        llvm::outs() << "=> me\n";
        me->dump();
        llvm::outs() << "=> dre_me\n";
        dre_me->dump();
        llvm::outs() << "=> dre\n";
        dre->dump();
        pb = new PortBinding(me, dre_me, dre,
                             instance_module_decl->getInstanceDecl(),
                             instance_module_decl->getInstanceName());
      }
      llvm::outs() << "Dump the port\n";
      pb->dump();
      llvm::outs() << "End dump of sun\n";
      instance_module_decl->addPortBinding(port_name, pb);
      instance_module_decl->dumpPortBinding();
    }
  }

  void dump() {
    // Dump out all the module instances.
    //
    auto instances_map{module_matcher_->getInstances()};

    for (const auto &inst : instances_map) {
      auto cxx_decl{inst.first};
      // Vector
      auto instance_list{inst.second};
      llvm::outs() << "[[NetlistMatcher]]: CXXRecordDecl* " << cxx_decl
                   << " name: " << cxx_decl->getName() << "\n";

      for (auto const &instance : instance_list) {
        ModuleDecl *mdecl{model_->getInstance(get<0>(instance))};
        Stmt *constructor_stmt{mdecl->getConstructorStmt()};
      }
      //
    }
  }
};  // namespace sc_ast_matchers
};  // namespace sc_ast_matchers

#endif
