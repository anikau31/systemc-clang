#ifndef _PORT_BINDING_H_
#define _PORT_BINDING_H_

using namespace systemc_clang;

#include "ArrayTypeUtils.h"
namespace systemc_clang {

using namespace sc_ast_matchers::utils::array_type;

class PortBinding {
 private:
  /// Reword
  //
  /// Caller
  clang::Expr *caller_expr_;
  clang::ArraySubscriptExpr *caller_array_expr_;
  const clang::MemberExpr *caller_instance_me_expr_;
  const clang::MemberExpr *caller_port_me_expr_;
  std::vector<llvm::APInt> caller_array_subscripts_;

  /// Callee
  clang::Expr *callee_expr_;
  const clang::MemberExpr *callee_port_me_expr_;  // port
  const clang::MemberExpr *callee_instance_me_expr_; //instance
  clang::ArraySubscriptExpr *callee_array_expr_;
  std::vector<llvm::APInt> callee_array_subscripts_;

  // Which of the two bindings is done (sc_main or ctor)
  //
  bool is_ctor_binding_;

  ///
  /// These private members are used when the port binding is done in the
  /// constructor.
  ///
  ///  sub_module_member.input(inS);
  /// callee_port_me_expr_   = input
  /// me_instance_    = sub_module_member
  /// me_arg_         = inS

  clang::MemberExpr *me_instance_;  // instance on which the binding is invoked
  clang::MemberExpr *me_arg_;       // argument to the port

  // The below private members are used when the port binding is found in the
  // sc_main.
  // There are only three pointers set, and rest are generated.
  // 1. port_member_expr_
  // 2. port_dref_
  // 3. port_parameter_dref_
  //
  clang::ArraySubscriptExpr *port_member_array_port_expr_;
  clang::DeclRefExpr *port_member_array_idx_dref_;

  // Instance information
  std::string caller_instance_name_;
  std::string caller_port_name_;
  std::string caller_type_name_;

  std::string callee_instance_name_;
  std::string callee_port_name_;
  std::string callee_type_name_;

  std::string instance_type_;
  std::string instance_var_name_;
  std::string instance_constructor_name_;
  // Declaration for the instance's type.
  clang::CXXRecordDecl *instance_type_decl_;

  // There are only two DeclRefExpr that you really need.
  // The rest will be generated from these.
  //
  // Declaration for the instance (FieldDecl/VarDecl)
  clang::Decl *instance_decl_;
  clang::DeclRefExpr *port_dref_;

  // Declaration for the argument for the port.
  std::string port_parameter_name_;
  const clang::Type *port_parameter_type_;
  std::string port_parameter_type_name_;
  std::string port_parameter_bound_to_var_name_;
  clang::DeclRefExpr *port_parameter_dref_;
  clang::DeclRefExpr *port_parameter_array_idx_dref_;
  clang::ArraySubscriptExpr *port_parameter_array_expr_;

 public:
  void setInstanceVarName(const std::string &n) { instance_var_name_ = n; }
  void setInstanceConstructorName(const std::string &n) {
    instance_constructor_name_ = n;
  }

  const std::string &getPortName() const { return callee_instance_name_; }
  const clang::MemberExpr *getCallerMemberExpr() const {
    return caller_instance_me_expr_;
  }
  const std::string &getInstanceType() const { return instance_type_; }
  const std::string &getInstanceVarName() const { return instance_var_name_; }
  const std::string &getInstanceConstructorName() const {
    return instance_constructor_name_;
  }
  clang::CXXRecordDecl *getInstanceTypeDecl() const {
    return instance_type_decl_;
  }
  clang::Decl *getInstanceDecl() const { return instance_type_decl_; }
  clang::DeclRefExpr *getPortDeclRefExpr() const { return port_dref_; }

  const std::string &getBoundToName() const { return port_parameter_name_; }
  const std::string &getBoundToParameterVarName() const {
    return port_parameter_bound_to_var_name_;
  }
  clang::DeclRefExpr *getBoundPortDeclRefExpr() const {
    return port_parameter_dref_;
  }
  const std::string toString() const {
    return getInstanceType() + " " + getInstanceVarName() + " " +
           getInstanceConstructorName() + " " + getBoundToName();
  }
  bool hasBoundToArrayParameter() const {
    return (port_parameter_array_idx_dref_ != nullptr);
  }
  bool hasPortArrayParameter() const {
    return (port_member_array_idx_dref_ != nullptr);
  }
  clang::DeclRefExpr *getPortArrayIndex() const {
    return port_member_array_idx_dref_;
  }
  clang::DeclRefExpr *getBoundToArrayIndex() const {
    return port_parameter_array_idx_dref_;
  }

  void dump() {
    llvm::outs() << "caller instance name : " << caller_instance_name_ << "  "
                 << "caller port name     : " << caller_port_name_ << "  "
                 << "\nCALLEE\n" 
                 << "callee port name     : " << callee_instance_name_ << "  "
                 << "callee port type name: " << callee_type_name_;
    /*
    llvm::outs() << "> inst type name: " << instance_type_
                 << ", inst var name : " << instance_var_name_
                 << ", port_type name: " << callee_type_name_
                 << ", port name     : " << callee_instance_name_
                 << ", port arg type : " << port_parameter_type_name_;

    if (port_member_array_idx_dref_) {
      port_member_array_idx_dref_->dump();
      llvm::outs() << ", port_array_idx: "
                   << port_member_array_idx_dref_->getNameInfo().getName();
    }

    llvm::outs() << ", port_bound_to_var_name : "
                 << port_parameter_bound_to_var_name_
                 << ", port arg      : " << port_parameter_name_;
    if (port_parameter_array_idx_dref_) {
      llvm::outs() << ", port_arg_array_idx: "
                   << port_parameter_array_idx_dref_->getNameInfo().getName();
    }
    */
    llvm::outs() << "\n";

    // llvm::outs() << "> port_name: " << callee_instance_name_ << " type: " <<
    // instance_type_
    // << " var_name: " << instance_var_name_
    // << " constructor_name: " << instance_constructor_name_
    // << " bound to " << port_parameter_name_ << "\n";
  }

  PortBinding(clang::Expr *caller_expr, clang::MemberExpr *caller_port_me_expr,
      clang::Expr *callee_expr, clang::MemberExpr *callee_port_me_expr)
      : caller_expr_{caller_expr}, callee_expr_{callee_expr}, caller_port_me_expr_{caller_port_me_expr}, callee_port_me_expr_{callee_port_me_expr}  {
    /// Cast to see if it's an array.
    caller_array_expr_ = dyn_cast<clang::ArraySubscriptExpr>(caller_expr);

    llvm::outs() << "==> Extract caller port name\n";
    if (caller_port_me_expr_) {
      caller_port_me_expr_->dump();
      caller_port_name_ = caller_port_me_expr_->getMemberNameInfo().getAsString();
    }

    /// If it is an array.
    if (caller_array_expr_) {
      caller_instance_me_expr_ = getArrayMemberExprName(caller_array_expr_);
      caller_array_subscripts_ = getArraySubscripts(caller_array_expr_);
    } else {
      caller_instance_me_expr_ = dyn_cast<clang::MemberExpr>(caller_expr);
    }

llvm::outs() << "==> Extract callee port name\n";
    if (callee_port_me_expr_) {
      callee_port_me_expr_->dump();
      callee_port_name_ = callee_port_me_expr_->getMemberNameInfo().getAsString();
    }

if (caller_instance_me_expr_) {
      caller_instance_name_ = caller_instance_me_expr_->getMemberNameInfo().getAsString();
    }


    // Callee
    callee_array_expr_ = dyn_cast<clang::ArraySubscriptExpr>(callee_expr);
    if (callee_array_expr_) {
      caller_instance_me_expr_ = getArrayMemberExprName(caller_array_expr_);
      callee_array_subscripts_ = getArraySubscripts(callee_array_expr_);
    } else {
      callee_instance_me_expr_ = dyn_cast<clang::MemberExpr>(callee_expr);
    }

if (callee_instance_me_expr_) {
      callee_instance_name_ = callee_instance_me_expr_->getMemberNameInfo().getAsString();
    }


    if (callee_port_me_expr_) {
      callee_port_me_expr_->dump();
    llvm::outs() << "Get port type\n";
    //callee_me_expr_->getMemberDecl()->dump();
    callee_port_me_expr_->getBase()->dump();
    callee_type_name_ = callee_port_me_expr_->getMemberDecl()
                          ->getType()
                          .getBaseTypeIdentifier()
                          ->getName();
    }
  }
  PortBinding(clang::ArraySubscriptExpr *port_array,
              clang::MemberExpr *me_ctor_port, clang::MemberExpr *me_instance,
              clang::MemberExpr *me_arg,
              clang::ArraySubscriptExpr *array_port_bound_to,
              clang::DeclRefExpr *array_idx)
      : port_member_array_port_expr_{port_array},
        port_member_array_idx_dref_{nullptr},
        callee_port_me_expr_{me_ctor_port},
        me_instance_{me_instance},
        me_arg_{me_arg},
        port_parameter_array_expr_{array_port_bound_to} {
    callee_instance_name_ = callee_port_me_expr_->getMemberNameInfo().getAsString();
    callee_type_name_ = me_ctor_port->getMemberDecl()
                          ->getType()
                          .getBaseTypeIdentifier()
                          ->getName();

    /// Get the port member's array index.
    if (port_member_array_port_expr_) {
      auto port_member_idx{
          port_member_array_port_expr_->getIdx()->IgnoreImpCasts()};
      if (auto idx_dref = dyn_cast<DeclRefExpr>(port_member_idx)) {
        llvm::outs() << "array_index: " << idx_dref->getNameInfo().getName()
                     << "\n";
        port_member_array_idx_dref_ = idx_dref;
      }
    }

    instance_var_name_ = me_instance_->getMemberNameInfo().getAsString();
    instance_type_ = me_instance_->getMemberDecl()
                         ->getType()
                         .getBaseTypeIdentifier()
                         ->getName();
    instance_type_decl_ = me_instance->getMemberDecl()
                              ->getType()
                              .getTypePtr()
                              ->getAsCXXRecordDecl();

    port_parameter_name_ = me_arg->getMemberNameInfo().getAsString();
    port_parameter_type_ = me_arg->getMemberDecl()->getType().getTypePtr();
    port_parameter_type_name_ =
        me_arg->getMemberDecl()->getType().getBaseTypeIdentifier()->getName();

    auto children{me_arg->children()};

    if (children.begin() != children.end()) {
      Stmt *kid{*children.begin()};
      if (clang::MemberExpr *
          member_expr_kid{dyn_cast<clang::MemberExpr>(kid)}) {
        port_parameter_bound_to_var_name_ =
            member_expr_kid->getMemberDecl()->getNameAsString();
      }
    }

    /// Port argument is an array. Need to find its index.
    //
    // port_arg_array_idx_ = idx_dref->getNameInfo().getName();
    port_parameter_array_idx_dref_ = array_idx;
  }

  // This is used for sc_main
  PortBinding(clang::MemberExpr *me, clang::DeclRefExpr *port_dref,
              clang::DeclRefExpr *port_arg_dref, clang::Decl *instance_decl,
              const std::string &instance_constructor_name)
      : is_ctor_binding_{false},
        // Used only for ctor bindings
        callee_port_me_expr_{nullptr},
        me_instance_{nullptr},
        me_arg_{nullptr},
        // Used only for sc_main bindings
        port_dref_{port_dref},
        port_parameter_dref_{port_arg_dref},
        instance_decl_{instance_decl},
        instance_constructor_name_{instance_constructor_name} {
    // Get the type of the instance (sc_module class name).

    callee_instance_name_ = me->getMemberDecl()->getNameAsString();
    callee_type_name_ =
        me->getMemberDecl()->getType().getBaseTypeIdentifier()->getName();

    instance_type_ =
        port_dref_->getDecl()->getType().getBaseTypeIdentifier()->getName();
    instance_decl_ = port_dref_->getDecl();
    instance_var_name_ = port_dref_->getFoundDecl()->getName();
    instance_type_decl_ =
        port_dref_->getDecl()->getType().getTypePtr()->getAsCXXRecordDecl();

    port_parameter_name_ = port_parameter_dref_->getFoundDecl()->getName();
    port_parameter_type_ =
        port_parameter_dref_->getDecl()->getType().getTypePtr();
    port_parameter_type_name_ = port_parameter_dref_->getDecl()
                                    ->getType()
                                    .getBaseTypeIdentifier()
                                    ->getName();
  };
};      // namespace systemc_clang
};      // namespace systemc_clang
#endif  // ifdef
