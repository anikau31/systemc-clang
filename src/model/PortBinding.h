#ifndef _PORT_BINDING_H_
#define _PORT_BINDING_H_

using namespace systemc_clang;

#include "ArrayTypeUtils.h"
namespace systemc_clang {

using namespace sc_ast_matchers::utils::array_type;

class PortBinding {
 private:
  typedef std::vector<const clang::Expr *> ArraySubscriptsExprType;
  // typedef std::vector<clang::Expr*> ArraySubscriptsType;
  /// Reword
  //
  /// Caller
  clang::Expr *caller_expr_;
  clang::ArraySubscriptExpr *caller_array_expr_;
  const clang::MemberExpr *caller_instance_me_expr_;
  clang::Expr *caller_port_array_expr_;
  ArraySubscriptsExprType caller_port_array_subscripts_;
  const clang::MemberExpr *caller_port_me_expr_;
  ArraySubscriptsExprType caller_array_subscripts_;

  /// Callee
  clang::Expr *callee_expr_;
  const clang::MemberExpr *callee_port_me_expr_;      // port
  const clang::MemberExpr *callee_instance_me_expr_;  // instance
  clang::ArraySubscriptExpr *callee_array_expr_;
  ArraySubscriptsExprType callee_array_subscripts_;

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
  std::string caller_instance_type_name_;

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
  const std::string getCallerInstanceName() const {
    return caller_instance_name_;
  }
  const std::string getCallerInstanceTypeName() const {
    return caller_instance_type_name_;
  }
  const std::string getCallerPortName() const { return caller_port_name_; }

  const std::string getCalleeInstanceName() const {
    return callee_instance_name_;
  }
  const std::string getCalleePortName() const { return callee_port_name_; }

  ArraySubscriptsExprType getCallerArraySubscripts() const {
    return caller_array_subscripts_;
  }

  ArraySubscriptsExprType getCallerPortArraySubscripts() const {
    return caller_port_array_subscripts_;
  }

  ArraySubscriptsExprType getCalleeArraySubscripts() const {
    return callee_array_subscripts_;
  }

  /// old

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
    std::string return_str{getCallerInstanceTypeName()};
    if (getCallerInstanceName() != "") {
      return_str = return_str + " " + getCallerInstanceName();
    }
    if (getInstanceConstructorName() != "") {
      return_str = return_str + " " + getInstanceConstructorName();
    }

    for (const auto &sub : getCallerArraySubscripts()) {
      auto is_int_lit{clang::dyn_cast<clang::IntegerLiteral>(sub)};
      auto is_dref_expr{clang::dyn_cast<clang::DeclRefExpr>(sub)};

      if (is_int_lit) {
        return_str += " " + is_int_lit->getValue().toString(32, true);
      }

      if (is_dref_expr) {
        return_str += " " + is_dref_expr->getNameInfo().getName().getAsString();
      }
    }

    /// Caller port name
    if (getCallerPortName() != "") {
      return_str = return_str + " " + getCallerPortName();
    }

    for (const auto &sub : getCallerPortArraySubscripts()) {
      auto is_int_lit{clang::dyn_cast<clang::IntegerLiteral>(sub)};
      auto is_dref_expr{clang::dyn_cast<clang::DeclRefExpr>(sub)};

      if (is_int_lit) {
        return_str += " " + is_int_lit->getValue().toString(32, true);
      }

      if (is_dref_expr) {
        return_str += " " + is_dref_expr->getNameInfo().getName().getAsString();
      }
    }

    if (getCalleeInstanceName() != "") {
      return_str = return_str + " " + getCalleeInstanceName();
    }

    for (const auto &sub : getCalleeArraySubscripts()) {
      auto is_int_lit{clang::dyn_cast<clang::IntegerLiteral>(sub)};
      auto is_dref_expr{clang::dyn_cast<clang::DeclRefExpr>(sub)};

      if (is_int_lit) {
        return_str += " " + is_int_lit->getValue().toString(32, true);
      }

      if (is_dref_expr) {
        return_str += " " + is_dref_expr->getNameInfo().getName().getAsString();
      }
    }

    if (getCalleePortName() != "") {
      return_str = return_str + " " + getCalleePortName();
    }
    return return_str;
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
    llvm::outs() << "caller instance type name : " << caller_instance_type_name_
                 << "  "
                 << "caller instance name : " << caller_instance_name_ << "  "
                 << "subscripts: ";
    for (const auto sub : caller_array_subscripts_) {
      auto int_lit{clang::dyn_cast<clang::IntegerLiteral>(sub)};
      if (int_lit) {
        llvm::outs() << " " << int_lit->getValue();
      }

      auto decl_ref_expr{clang::dyn_cast<clang::DeclRefExpr>(sub)};
      if (decl_ref_expr) {
        llvm::outs() << " "
                     << decl_ref_expr->getNameInfo().getName().getAsString();
      }
    }

    llvm::outs() << "\n";

    llvm::outs() << "caller port name     : " << caller_port_name_ << "  "
                 << "subscripts: ";
    for (const auto &sub : caller_port_array_subscripts_) {
      auto int_lit{clang::dyn_cast<clang::IntegerLiteral>(sub)};
      if (int_lit) {
        llvm::outs() << " " << int_lit->getValue();
      }

      auto decl_ref_expr{clang::dyn_cast<clang::DeclRefExpr>(sub)};
      if (decl_ref_expr) {
        llvm::outs() << " "
                     << decl_ref_expr->getNameInfo().getName().getAsString();
      }
    }

    llvm::outs() << "\nCALLEE\n"
                 << "callee instance name  : " << callee_instance_name_ << "  "
                 << "callee port name      : " << callee_port_name_ << "  "
                 << "subscripts: ";
    for (const auto sub : callee_array_subscripts_) {
      auto int_lit{clang::dyn_cast<clang::IntegerLiteral>(sub)};
      if (int_lit) {
        llvm::outs() << " " << int_lit->getValue();
      }

      auto decl_ref_expr{clang::dyn_cast<clang::DeclRefExpr>(sub)};
      if (decl_ref_expr) {
        llvm::outs() << " "
                     << decl_ref_expr->getNameInfo().getName().getAsString();
      }
    }

    llvm::outs() << "\n";

    // llvm::outs() << "> port_name: " << callee_instance_name_ << " type: " <<
    // instance_type_
    // << " var_name: " << instance_var_name_
    // << " constructor_name: " << instance_constructor_name_
    // << " bound to " << port_parameter_name_ << "\n";
  }

  PortBinding(clang::Expr *caller_expr, clang::Expr *caller_port_expr,
              clang::MemberExpr *caller_port_me_expr, clang::Expr *callee_expr,
              clang::MemberExpr *callee_port_me_expr)
      : caller_expr_{caller_expr},
        callee_expr_{callee_expr},
        caller_port_array_expr_{caller_port_expr},
        caller_port_me_expr_{caller_port_me_expr},
        callee_port_me_expr_{callee_port_me_expr} {
    /// Cast to see if it's an array.
    caller_array_expr_ = dyn_cast<clang::ArraySubscriptExpr>(caller_expr);

    llvm::outs() << "==> Extract caller port name\n";
    // Check to see if the port is initself an array
    if (caller_port_array_expr_) {
      caller_port_array_subscripts_ =
          getArraySubscripts(caller_port_array_expr_);
    }

    if (caller_port_me_expr_) {
      // caller_port_me_expr_->dump();
      caller_port_name_ =
          caller_port_me_expr_->getMemberNameInfo().getAsString();
    }

    /// If it is an array.
    if (caller_array_expr_) {
      caller_instance_me_expr_ = getArrayMemberExprName(caller_array_expr_);
      caller_array_subscripts_ = getArraySubscripts(caller_array_expr_);
    } else {
      caller_instance_me_expr_ = dyn_cast<clang::MemberExpr>(caller_expr);
    }

    if (caller_instance_me_expr_) {
      caller_instance_name_ =
          caller_instance_me_expr_->getMemberNameInfo().getAsString();
      llvm::outs() << "========= CALLER ME EXPR ======== \n";
      caller_instance_me_expr_->dump();
      caller_instance_type_name_ = caller_instance_me_expr_->getMemberDecl()
                                       ->getType()
                                       .getBaseTypeIdentifier()
                                       ->getName();
      llvm::outs() << "========= END CALLER ME EXPR ======== \n";
    }

    llvm::outs() << "==> Extract callee port name\n";
    if (callee_port_me_expr_) {
      // callee_port_me_expr_->dump();
      callee_port_name_ =
          callee_port_me_expr_->getMemberNameInfo().getAsString();
      llvm::outs() << " **** callee_port_name_: " << callee_port_name_ << "\n";
    }

    // Callee is an array
    callee_array_expr_ = dyn_cast<clang::ArraySubscriptExpr>(callee_expr);
    if (callee_array_expr_) {
      llvm::outs() << "extract callee name\n";
      callee_instance_me_expr_ = getArrayMemberExprName(callee_array_expr_);
      callee_array_subscripts_ = getArraySubscripts(callee_array_expr_);
    } else {
      callee_instance_me_expr_ = dyn_cast<clang::MemberExpr>(callee_expr);
    }
    if (callee_instance_me_expr_) {
      callee_instance_name_ =
          callee_instance_me_expr_->getMemberNameInfo().getAsString();
    }
  }

};      // namespace systemc_clang
};      // namespace systemc_clang
#endif  // ifdef
