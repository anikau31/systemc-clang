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
  Model *model_;
  const InstanceMatcher *instance_matcher_;
  ModuleDeclarationMatcher *module_matcher_;
  std::string top_;

  ModuleInstance *findModuleDeclInstance(clang::Decl *decl) {
    auto instances{model_->getInstances()};
    auto found_inst_it = std::find_if(
        instances.begin(), instances.end(), [decl](const auto &instance) {
          clang::Decl *i{instance->getInstanceDecl()};
          return (instance->getInstanceDecl() == decl);
        });

    if (found_inst_it != instances.end()) {
      return *found_inst_it;
    }
    return nullptr;
  }

 public:
  void registerMatchers(MatchFinder &finder, systemc_clang::Model *model,
                        ModuleDeclarationMatcher *module_matcher) {
    model_ = model;
    module_matcher_ = module_matcher;
    instance_matcher_ = &(module_matcher_->getInstanceMatcher());

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

    /// Either a port (with sc_port base class) or module (with sc_module base class) can be bound.
    /// This will match if either one of those is the type.
    const auto not_sc_sensitive = unless(hasType(cxxRecordDecl(isDerivedFrom("sc_core::sc_sensitive")))); 
    const auto not_sc_event_finder = unless(hasType(cxxRecordDecl(isDerivedFrom("sc_core::sc_event_finder")))); 

    const auto has_allowed_types =   
      anyOf(
          hasType(cxxRecordDecl(isDerivedFrom(hasName("sc_core::sc_module")))),
          hasType(cxxRecordDecl(isDerivedFrom(hasName("sc_core::sc_port")))),
          hasType(cxxRecordDecl(isDerivedFrom(hasName("sc_core::sc_signal"))))
        );

    const auto caller_array_subscript =
      arraySubscriptExpr(
          has_allowed_types,
        hasParent(
          memberExpr().bind("caller_port_me_expr")
          )
        ).bind("caller_expr") ;

    
    /// Callers are nested within the CXXOperatorCallExpr; hence, the hasDescendant().
    const auto match_callers = 
      anyOf( 
        hasDescendant(caller_array_subscript) //hasDescendant
      ,
        /// This priority is imporant. anyOf() priority is in the order of
        /// specification. Thus, we check if there is an ArraySubscriptExpr for the
        /// port, and only if it is not then we check a non-array port. The caller port
        /// could also be a array, so we need to check if the ancestor has an
        /// ArraySubscriptExpr. This should have worked better with using optionally(),
        /// but it didn't work.
        hasDescendant(
          memberExpr(
            hasDescendant(
              memberExpr(has_allowed_types).bind("caller_expr")
              ) //hasDescendant
              ,
              // The port could be an array as well.
              hasAncestor(arraySubscriptExpr().bind("caller_array_port_expr"))
            ).bind("caller_port_me_expr"))

      ,
        hasDescendant(
          memberExpr(
            hasDescendant(
              memberExpr(has_allowed_types).bind("caller_expr")
              ) //hasDescendant
              //,
              // The port could be an array as well.
              //optionally(hasAncestor(arraySubscriptExpr().bind("caller_array_port_expr")))
            ).bind("caller_port_me_expr"))
      );

    const auto callee_array_subscript =
      arraySubscriptExpr(
          has_allowed_types,
        hasParent(
          memberExpr().bind("callee_port_me_expr")
          )
        ).bind("callee_expr") ;



    /// Callees are children of the cxxOperatorCallExpr; hence, the use of has().
    const auto match_callees = 
      anyOf( 
        ignoringImplicit(has(callee_array_subscript))
      ,
        ignoringImplicit(expr().bind("callee_expr") ) //hasDescendant
      );


    auto not_operator_less_than = unless(callee(cxxMethodDecl(hasName("operator<<"))));

    auto test_match_ctor_decl = 
      namedDecl(
            has(compoundStmt(
                    forEachDescendant(
                        cxxOperatorCallExpr(
                          not_operator_less_than, 
                          /// The memberExpr gives the port name.
                          /// The arraySubscriptExpr gives the ArraySubscriptExpr to the array.
                          match_callers
                        ,
                          hasArgument(1, 
                            match_callees
                          )

                      ).bind("opcall")))
                    .bind("compoundstmt"))
            ).bind("named_decl");

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

    /// Rework

    auto named_decl{const_cast<clang::NamedDecl *>(
        result.Nodes.getNodeAs<clang::NamedDecl>("named_decl"))};
    auto opcall{const_cast<clang::CXXOperatorCallExpr *>(
        result.Nodes.getNodeAs<clang::CXXOperatorCallExpr>("opcall"))};
    opcall->dump();
    llvm::outs() << " get type \n";
    opcall->getDirectCallee()->dump();
    llvm::outs() << "name: " << opcall->getDirectCallee()->getNameAsString()
                 << "\n";

    auto caller_expr{const_cast<clang::Expr *>(
        result.Nodes.getNodeAs<clang::Expr>("caller_expr"))};
    auto callee_expr{const_cast<clang::Expr *>(
        result.Nodes.getNodeAs<clang::Expr>("callee_expr"))};

    auto caller_array_expr{const_cast<clang::ArraySubscriptExpr *>(
        result.Nodes.getNodeAs<clang::ArraySubscriptExpr>("caller_expr"))};
    auto callee_array_expr{const_cast<clang::ArraySubscriptExpr *>(
        result.Nodes.getNodeAs<clang::ArraySubscriptExpr>("callee_expr"))};

    auto caller_array_port_expr{const_cast<clang::ArraySubscriptExpr *>(
        result.Nodes.getNodeAs<clang::ArraySubscriptExpr>(
            "caller_array_port_expr"))};

    /// MemberExpr that gives the method name (port name).
    auto caller_port_me_expr{const_cast<clang::MemberExpr *>(
        result.Nodes.getNodeAs<clang::MemberExpr>("caller_port_me_expr"))};
    auto callee_port_me_expr{const_cast<clang::MemberExpr *>(
        result.Nodes.getNodeAs<clang::MemberExpr>("callee_port_me_expr"))};

    // if (caller_array_expr) {
    // llvm::outs() << "=========== @@@@@@@@@@@@@@@@@@@@@ CALLER ARRAY EXPR\n";
    // }
    // if (caller_expr) {
    // llvm::outs() << "=========== @@@@@@@@@@@@@@@@@@@@@ CALLER EXPR\n";
    // caller_expr->dump();
    // }
    // if (caller_array_port_expr) {
    // llvm::outs()
    // << "=========== @@@@@@@@@@@@@@@@@@@@@ CALLER ARRAY PORT EXPR\n";
    // caller_array_port_expr->dump();
    // }
    //
    // if (callee_array_expr) {
    // llvm::outs()
    // << "=========== @@@@@@@@@@@@@@@@@@@@@ CALLEEEEE ARRAAY EXPR\n";
    // callee_expr->dump();
    //
    // } else if (callee_expr) {
    // llvm::outs() << "=========== @@@@@@@@@@@@@@@@@@@@@ CALLEEEE EXPR\n";
    // callee_expr->dump();
    // }

    if (caller_expr && callee_expr) {
      PortBinding *pb{new PortBinding(caller_expr, caller_array_port_expr,
                                      caller_port_me_expr, callee_expr,
                                      callee_port_me_expr)};

      pb->dump();

      llvm::outs() << "Try to add it into the module\n";
      caller_expr->dump();

      // Have to add the port binding into the appropriate module instance.
      //
      // Get the appropriate MemberExpr for either array or non-array.
      const clang::MemberExpr *caller_me_expr{nullptr};
      if (caller_array_expr) {
        caller_me_expr = getArrayMemberExprName(caller_array_expr);
      } else if (caller_expr) {
        caller_me_expr = clang::dyn_cast<clang::MemberExpr>(caller_expr);
      }

      if (caller_me_expr) {
        caller_me_expr->dump();
        auto caller_instance_decl{caller_me_expr->getMemberDecl()};
        auto caller_instance_type{caller_instance_decl->getType().getTypePtr()};
        ModuleInstance *instance_module_decl{
            findModuleDeclInstance(caller_instance_decl)};
        llvm::outs() << "INSTANCE MODULE :"
                     << instance_module_decl->getInstanceName() << "\n";
        llvm::outs() << " port name : " << pb->getCallerPortName() << "\n";
        llvm::outs() << " PARENT@@@@ : "
                     << instance_module_decl->getInstanceInfo()
                            .getParentDecl()
                            ->getName()
                     << "\n";
        // Get the parent ModuleInstance and insert the port binding into that one.
        ModuleInstance *parent_decl{findModuleDeclInstance(
            instance_module_decl->getInstanceInfo().getParentDecl())};
        llvm::outs() << " PARENT@@@@ INST NAME: "
                     << parent_decl->getInstanceName() << "\n";

        /// This string is necessary since addPortbinding stores a map of string
        /// => Portbinding. Thus just using port name is not unique.
        std::string binding_name{pb->getCallerInstanceName() +
                                 pb->getCallerPortName()};
        parent_decl->addPortBinding(binding_name, pb);
        pb->setInstanceConstructorName(instance_module_decl->getInstanceName());
        // instance_module_decl->dumpPortBinding();
      }
    }
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
  }
};
};  // namespace sc_ast_matchers

#endif
