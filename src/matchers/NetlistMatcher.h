#ifndef _NETLIST_MATCHER_H_
#define _NETLIST_MATCHER_H_

#include "clang/ASTMatchers/ASTMatchers.h"

#include "Matchers.h"
#include "ArrayTypeUtils.h"

#include "PortBinding.h"
#include "SensitivityMatcher.h"

using namespace clang::ast_matchers;

#undef DEBUG_TYPE
#define DEBUG_TYPE "NetlistMatcher"

namespace sc_ast_matchers {

using namespace sc_ast_matchers::utils::array_type;
///////////////////////////////////////////////////////////////////////////////
///
/// Class NetlistMatcher
///
///
///////////////////////////////////////////////////////////////////////////////
class NetlistMatcher : public MatchFinder::MatchCallback {
 private:
  // const ModuleDeclarationMatcher::DeclarationsToInstancesMapType
  // *decl_instance_map_;
  systemc_clang::Model *model_;
  const InstanceMatcher *instance_matcher_;
  ModuleDeclarationMatcher *module_matcher_;
  std::string top_;

  systemc_clang::ModuleDecl *findModuleDeclInstance(clang::Decl *decl) {
    // This is the instance type decl
    // llvm::outs() << "=> findModuleDeclInstance: Looking for: " << decl <<
    // "\n";
    auto instances{model_->getInstances()};
    // for (auto const &inst: model_->getInstances()) {
    // auto incomplete{element.first};
    // llvm::outs() << "=> incomplete: " << incomplete->getName() << "\n";
    // auto instance_list{element.second};

    // for (auto const &inst : instance_list) {
    // This is the instance type decl.
    //   clang::Decl *inst_decl{inst->getInstanceDecl()};
    // llvm::outs() << "=> find: " << decl << " == " << inst_decl << "\n";
    //}
    //
    auto found_inst_it = std::find_if(
        instances.begin(), instances.end(), [decl](const auto &instance) {
          clang::Decl *i{instance->getInstanceDecl()};
          return (instance->getInstanceDecl() == decl);
        });

    if (found_inst_it != instances.end()) {
      // llvm::outs() << "=> found the iterator\n";
      return *found_inst_it;
    }
    //}
    return nullptr;
  }

 public:
  void registerMatchers(MatchFinder &finder, systemc_clang::Model *model,
                        ModuleDeclarationMatcher *module_matcher) {
    /* clang-format off */

    model_ = model;
    module_matcher_ = module_matcher;
    instance_matcher_ = & (module_matcher_->getInstanceMatcher());
    
    /* clang-format off */

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
               ) //hasDescendant
         ).bind("callexpr")
       )
     ).bind("functiondecl");


    const auto caller_array_subscript =
                                  arraySubscriptExpr(
                                    hasParent(
                                      memberExpr().bind("caller_port_me_expr")
                                      )
                                    ).bind("caller_expr") ;


/*
    const auto caller_array_subscript =  memberExpr(ignoringImplicit(
                                    has(arraySubscriptExpr().bind("caller_expr")  ) )
                                  ).bind("caller_port_me_expr");
                                  */

    const auto match_callers = 
      anyOf( 
        hasDescendant(caller_array_subscript) //hasDescendant
      ,
        hasDescendant(expr().bind("caller_expr") ) //hasDescendant
      );

    const auto callee_array_subscript =
                                  arraySubscriptExpr(
                                    hasParent(
                                      memberExpr().bind("callee_port_me_expr")
                                      )
                                    ).bind("callee_expr") ;


/*
    const auto callee_array_subscript =  
      memberExpr(ignoringImplicit(
        has(arraySubscriptExpr().bind("callee_expr")  ) )
      ).bind("callee_port_me_expr");
      */


    const auto match_callees = 
      anyOf( 
        ignoringImplicit(has(callee_array_subscript))
      ,
        ignoringImplicit(expr().bind("callee_expr") ) //hasDescendant
      );


    auto test_match_ctor_decl = 
      namedDecl(
            has(compoundStmt(
                    forEachDescendant(
                        cxxOperatorCallExpr(
                          /// The memberExpr gives the port name.
                          /// The arraySubscriptExpr gives the ArraySubscriptExpr to the array.
                          match_callers
                        ,
                          hasArgument(1, 
                            match_callees
                          )

                            // anyOf(
                              // hasDescendant(arraySubscriptExpr(
                                          // hasDescendant(memberExpr(has(memberExpr().bind("memberexpr_instance"))).bind("memberexpr_port"))
                                          // )
                                          // .bind("array_expr_port"))
                              // ,
                                  // hasDescendant(memberExpr(has(memberExpr().bind( "memberexpr_instance")))
                                          // .bind("memberexpr_port")))  // anyOf
//
                            // Arguments
                            // 1. Has MemberExpr as a descendant.
                            //   1.a It must have an implicitCastExpr as a
                            //   parent 1.b But not a MemberExpr (This is
                            //   because it matches with the port others.
                            // , hasDescendant(
                            // memberExpr(hasParent(implicitCastExpr()),
                            // unless(hasDescendant(memberExpr())))
                            // .bind("memberexpr_arg"))
                            //

                            // Get the memberExpr for the argument.
                            // ,
                            // hasArgument(1,
                                // anyOf(
                                  // arraySubscriptExpr().bind("array_port_arg")
                                  // ,
                                  // expr().bind("port_arg")
                                 // )  // anyOf
                            // )  // hasArgument
                      ).bind("opcall")))
                    .bind("compoundstmt")))
            .bind("functiondecl");


    auto match_ctor_decl =
        namedDecl(
            has(compoundStmt(
                    forEachDescendant(
                        cxxOperatorCallExpr(
                            // callExpr(
                            // Port
                            // 1. MemberExpr has a descendant that is a
                            // MemberExpr The first MemberExpr gives the
                            //{}/ port bound to information. The second
                            // MemberExpr gives the instance information
                            // to whom the port belongs.

                            anyOf(hasDescendant(arraySubscriptExpr(
                                          hasDescendant(memberExpr(has(memberExpr().bind("memberexpr_instance"))).bind(
                                              "memberexpr_port")))
                                          .bind("array_expr_port")),
                                  hasDescendant(memberExpr(has(memberExpr().bind(
                                                     "memberexpr_instance")))
                                          .bind("memberexpr_port")))  // anyOf

                            // Arguments
                            // 1. Has MemberExpr as a descendant.
                            //   1.a It must have an implicitCastExpr as a
                            //   parent 1.b But not a MemberExpr (This is
                            //   because it matches with the port others.
                            // , hasDescendant(
                            // memberExpr(hasParent(implicitCastExpr()),
                            // unless(hasDescendant(memberExpr())))
                            // .bind("memberexpr_arg"))
                            //

                            // Get the memberExpr for the argument.
                            ,
                            hasArgument(1, 
                                anyOf(
                                  arraySubscriptExpr().bind("array_port_arg"),
                                         expr().bind("port_arg")
                                 )  // anyOf
                            )  // hasArgument
                      ).bind("callexpr")))
                    .bind("compoundstmt")))
            .bind("functiondecl");

    /* clang-format on */

    // Add the two matchers.
    // finder.addMatcher(match_sc_main_callexpr, this);
    // finder.addMatcher(match_ctor_decl, this);
    finder.addMatcher(test_match_ctor_decl, this);
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
    LLVM_DEBUG(
        llvm::dbgs()
            << "#### ============ NETLIST HAD A MATCH ============ ####\n";);
    bool is_ctor_binding{true};

    /// Rework

    auto opcall{const_cast<clang::Expr *>(
        result.Nodes.getNodeAs<clang::Expr>("opcall"))};
    opcall->dump();

    auto caller_expr{const_cast<clang::Expr *>(
        result.Nodes.getNodeAs<clang::Expr>("caller_expr"))};
    auto callee_expr{const_cast<clang::Expr *>(
        result.Nodes.getNodeAs<clang::Expr>("callee_expr"))};

    auto caller_array_expr{const_cast<clang::ArraySubscriptExpr *>(
        result.Nodes.getNodeAs<clang::ArraySubscriptExpr>("caller_expr"))};
    auto callee_array_expr{const_cast<clang::ArraySubscriptExpr *>(
        result.Nodes.getNodeAs<clang::ArraySubscriptExpr>("callee_expr"))};

    /// MemberExpr that gives the method name (port name).
    auto caller_port_me_expr{const_cast<clang::MemberExpr *>(
        result.Nodes.getNodeAs<clang::MemberExpr>("caller_port_me_expr"))};
    auto callee_port_me_expr{const_cast<clang::MemberExpr *>(
        result.Nodes.getNodeAs<clang::MemberExpr>("callee_port_me_expr"))};

    if (caller_array_expr) {
      llvm::outs() << "=========== @@@@@@@@@@@@@@@@@@@@@ CALLER ARRAY EXPR \n";
      caller_array_expr->dump();
      if (caller_port_me_expr) {
        caller_port_me_expr->dump();
      }

    } else if (caller_expr) {
      llvm::outs() << "=========== @@@@@@@@@@@@@@@@@@@@@ CALLER EXPR\n";
      caller_expr->dump();

      if (auto me_caller_expr =
              clang::dyn_cast<clang::MemberExpr>(caller_expr)) {
        // me_caller_expr->dump();
      }
    }

    /// Callee handling.
    ///
    /// Callee array expressions.
    if (callee_array_expr) {
      llvm::outs()
          << "=========== @@@@@@@@@@@@@@@@@@@@@ CALLEEEEE ARRAAY EXPR\n";
      callee_expr->dump();
      if (callee_port_me_expr) {
        callee_port_me_expr->dump();
      }

    } else if (callee_expr) {
      llvm::outs() << "=========== @@@@@@@@@@@@@@@@@@@@@ CALLEEEE EXPR\n";
      callee_expr->dump();
    }

    if (caller_expr || callee_expr) {
      PortBinding *pb{new PortBinding(caller_expr, caller_port_me_expr,
                                      callee_expr, callee_port_me_expr)};
      pb->dump();
    }

    /*

    auto me{const_cast<clang::MemberExpr *>(
        result.Nodes.getNodeAs<clang::MemberExpr>("memberexpr"))};
    auto dre_me{const_cast<clang::DeclRefExpr *>(
        result.Nodes.getNodeAs<clang::DeclRefExpr>(
            "declrefexpr_in_memberexpr"))};
    auto dre{const_cast<clang::DeclRefExpr *>(
        result.Nodes.getNodeAs<clang::DeclRefExpr>("declrefexpr"))};
    auto fd{const_cast<clang::FunctionDecl *>(
        result.Nodes.getNodeAs<clang::FunctionDecl>("functiondecl"))};

    //////////////////////////////////////////////////////
    /// TESTING
    /// Check if expr_port is an ArraySubscriptExpr or MemberExpr
    // auto expr_port{
    // const_cast<Expr *>(result.Nodes.getNodeAs<Expr>("memberexpr_port"))};
    //
    auto mexpr_port{const_cast<clang::MemberExpr *>(
        result.Nodes.getNodeAs<clang::MemberExpr>("memberexpr_port"))};


    auto mexpr_instance{const_cast<clang::MemberExpr *>(
        result.Nodes.getNodeAs<clang::MemberExpr>("memberexpr_instance"))};
    auto mexpr_arg{const_cast<clang::MemberExpr *>(
        result.Nodes.getNodeAs<clang::MemberExpr>("memberexpr_arg"))};

    // DEBUGGING
    //
    //
    //

    /// Check if it is an array.
    /// If it is array then the way to get the MemeberExpr with the name would
    /// be different. Otherwise, it can just be an expression.
    auto array_port_arg{const_cast<clang::ArraySubscriptExpr *>(
        result.Nodes.getNodeAs<clang::ArraySubscriptExpr>("array_port_arg"))};

    auto fd_port_arg{const_cast<clang::MemberExpr *>(
        result.Nodes.getNodeAs<clang::MemberExpr>("port_arg"))};
    LLVM_DEBUG(llvm::dbgs() << "@@@@@@@@@@@@@@@@@@@@@@@@ CHECK IF PORT ARG "
                               "@@@@@@@@@@@@@@@\n";);
    LLVM_DEBUG(fd_port_arg->dump(););
    if (fd_port_arg) {
      LLVM_DEBUG(llvm::dbgs() << " ########################## PORT ARG "
                                 "####################\n";);
      LLVM_DEBUG(fd_port_arg->dump(););

      mexpr_arg = fd_port_arg;
    }

    clang::DeclRefExpr *port_arg_array_idx_{nullptr};
    if (array_port_arg) {
      LLVM_DEBUG(llvm::dbgs() << "Array subscript as argument\n";);
      LLVM_DEBUG(llvm::dbgs() << "Base\n";);
      auto base{array_port_arg->getBase()->IgnoreImpCasts()};
      /// Get the argument's name.
      /// For example as[4], this is trying to get the as.
      if (auto arg_name = dyn_cast<MemberExpr>(base)) {
        LLVM_DEBUG(llvm::dbgs() << "argument NAME: "
                                << arg_name->getMemberDecl()->getNameAsString()
                                << "\n";);
        mexpr_arg = arg_name;
      }

      LLVM_DEBUG(llvm::dbgs() << "Index\n";);
      auto idx{array_port_arg->getIdx()->IgnoreImpCasts()};

      if (auto idx_dref = dyn_cast<clang::DeclRefExpr>(idx)) {
        LLVM_DEBUG(llvm::dbgs() << "array_index: "
                                << idx_dref->getNameInfo().getName() << "\n";);
        port_arg_array_idx_ = idx_dref;
      }
    }

    std::string port_name{};
    if (mexpr_port && mexpr_instance && mexpr_arg) {
      is_ctor_binding = true;
      LLVM_DEBUG(llvm::dbgs() << "#### Found MemberExpr: "
                              << "\n";);
      port_name = mexpr_port->getMemberNameInfo().getAsString();
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
      /// Suppose we have s_port.data as the binding.
      /// The AST first represents .data as a MemberExpr, and then a child of
      /// that is another MemberExpr that has the s_port.
      ///
      /// Find the first child of the mexpr_arg.
      auto children{mexpr_arg->children()};

      std::string port_bound_to_var_name{};
      if (children.begin() != children.end()) {
        LLVM_DEBUG(llvm::dbgs() << " ========= KID \n";);
        Stmt *kid{*children.begin()};
        if (clang::MemberExpr *
            member_expr_kid{dyn_cast<clang::MemberExpr>(kid)}) {
          LLVM_DEBUG(member_expr_kid->dump(););
          port_bound_to_var_name =
              member_expr_kid->getMemberDecl()->getNameAsString();
        }
      }

      LLVM_DEBUG(llvm::dbgs() << "=============++> port_bound_to_name: "
                              << port_bound_to_var_name << "\n";);
      LLVM_DEBUG(mexpr_arg->getMemberDecl()->dump(););
      LLVM_DEBUG(mexpr_instance->dump(););

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

    // std::string port_name{};
    if (me && dre_me && dre) {
      is_ctor_binding = false;
      port_name = me->getMemberDecl()->getNameAsString();
    }

    clang::Decl *instance_decl{nullptr};
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

    systemc_clang::ModuleDecl *instance_module_decl{
        findModuleDeclInstance(instance_decl)};
    if (!instance_module_decl) {
      LLVM_DEBUG(llvm::dbgs() << "@@@@@ No instance module decl found \n";);
    }

    if (instance_module_decl) {
      auto instance_constructor_name = instance_module_decl->getInstanceName();
      LLVM_DEBUG(llvm::dbgs() << "@@@@@@ instance constructor name: "
                              << instance_constructor_name << "\n";);

      PortBinding *pb{nullptr};

      if (is_ctor_binding) {
        LLVM_DEBUG(llvm::dbgs() << "=> CTOR binding\n";);
        LLVM_DEBUG(llvm::dbgs() << "=> port name: " << port_name << "\n";);
        pb = new PortBinding(caller_array_port, mexpr_port, mexpr_instance,
                             mexpr_arg, array_port_arg, port_arg_array_idx_);
        pb->setInstanceConstructorName(instance_module_decl->getInstanceName());
      } else {
        LLVM_DEBUG(llvm::dbgs() << "=> found instance in sc_main\n";);
        LLVM_DEBUG(llvm::dbgs() << "=> me\n";);
        LLVM_DEBUG(me->dump(););
        LLVM_DEBUG(llvm::dbgs() << "=> dre_me\n";);
        LLVM_DEBUG(dre_me->dump(););
        LLVM_DEBUG(llvm::dbgs() << "=> dre\n"; dre->dump(););

        pb = new PortBinding(me, dre_me, dre,
                             instance_module_decl->getInstanceDecl(),
                             instance_module_decl->getInstanceName());
      }
      LLVM_DEBUG(llvm::dbgs() << "Dump the port\n"; pb->dump();
                 llvm::dbgs() << "End dump of sun\n";);

      instance_module_decl->addPortBinding(port_name, pb);
      instance_module_decl->dumpPortBinding();
    }
    */
  }

  void dump() {
    // Dump out all the module instances.
    //
    auto instances{model_->getInstances()};

    for (auto const &inst : instances) {
      auto port_bindings{inst->getPortBindings()};

      for (auto const &p : port_bindings) {
        p.second->dump();
      }
    }

    /*
    auto instances_map{module_matcher_->getInstances()};

    for (const auto &inst : instances_map) {
      auto cxx_decl{inst.first};
      // Vector
      auto instance_list{inst.second};
      llvm::outs() << "[[NetlistMatcher]]: CXXRecordDecl* " << cxx_decl
                   << " name: " << cxx_decl->getName() << "\n";

      for (auto const &instance : instance_list) {
        systemc_clang::ModuleDecl *mdecl{model_->getInstance(get<0>(instance))};
        // Stmt *constructor_stmt{mdecl->getConstructorStmt()};
        auto port_bindings{mdecl->getPortBindings()};

        for (auto const &p : port_bindings) {
          p.second->dump();
        }
      }
    }
    */
  }
};
};  // namespace sc_ast_matchers

#endif
