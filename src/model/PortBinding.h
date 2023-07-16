#ifndef _PORT_BINDING_H_
#define _PORT_BINDING_H_

using namespace systemc_clang;

#include "ArrayTypeUtils.h"
#include "APIntUtils.h"
namespace systemc_clang {

using namespace sc_ast_matchers::utils::array_type;

class PortBinding {
 public:
  typedef std::vector<const clang::Expr *> ArraySubscriptsExprType;

 private:
  //
  /// Caller
  const clang::Expr *caller_expr_;
  const clang::ArraySubscriptExpr *caller_array_expr_;
  const clang::MemberExpr *caller_instance_me_expr_;
  const clang::Expr *caller_port_array_expr_;
  ArraySubscriptsExprType caller_port_array_subscripts_;
  const clang::MemberExpr *caller_port_me_expr_;
  ArraySubscriptsExprType caller_array_subscripts_;

  /// Callee
  const clang::Expr *callee_expr_;
  const clang::MemberExpr *callee_port_me_expr_;      // port
  const clang::MemberExpr *callee_instance_me_expr_;  // instance
  const clang::ArraySubscriptExpr *callee_array_expr_;
  ArraySubscriptsExprType callee_array_subscripts_;

  /// We no longer support sc_main parsing.

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
  const clang::CXXRecordDecl *instance_type_decl_;

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

const clang::Expr *getCallerExpr() const { return caller_expr_; }
const clang::MemberExpr *getCallerInstanceMeExpr() const { return caller_instance_me_expr_; }
const clang::Expr *getCallerPortArrayExpr() const { return caller_port_array_expr_; }
const clang::MemberExpr *getCallerPortMeExpr() const { return caller_port_me_expr_; }

const clang::Expr *getCalleeExpr() const { return callee_expr_; }
const clang::MemberExpr *getCalleeInstanceMeExpr() const { return callee_instance_me_expr_; }
const clang::MemberExpr *getCalleePortMeExpr() const { return callee_port_me_expr_; }
 
  void setInstanceVarName(const std::string &name) {
    instance_var_name_ = name;
  }
  void setInstanceConstructorName(const std::string &name) {
    instance_constructor_name_ = name;
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
  const clang::CXXRecordDecl *getInstanceTypeDecl() const {
    return instance_type_decl_;
  }

  /// Convert the port binding information into a single string.
  ///
  /// This is mainly used in the regression tests.
  /// The returned string is compared with what is expected.
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
        //return_str += " " + is_int_lit->getValue().toString(to_str, 32, true);
        return_str += " " + systemc_clang::utils::apint::toString(is_int_lit->getValue());
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
        //return_str += " " + is_int_lit->getValue().toString(to_str, 32, true);
        return_str += " " + systemc_clang::utils::apint::toString(is_int_lit->getValue());
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
        //return_str += " " + is_int_lit->getValue().toString(to_str,32, true);
        return_str += " " + systemc_clang::utils::apint::toString(is_int_lit->getValue());
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

  /// Dumps to llvm::outs() the recorded port binding information.
  ///
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
  }

  /// Constructor that records the port binding.
  ///
  /// \param caller_expr provides access to the caller information irrespective
  /// of whether it is an array or not. \param caller_port_expr provides access
  /// to the caller's port.
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

    LLVM_DEBUG(llvm::dbgs() << "==> Extract caller port name\n";);
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
      LLVM_DEBUG(
          llvm::dbgs() << "========= CALLER ME EXPR ======== \n";
      caller_instance_me_expr_->dump();
      );
      caller_instance_type_name_ = caller_instance_me_expr_->getMemberDecl()
                                       ->getType()
                                       .getBaseTypeIdentifier()
                                       ->getName().str();
    }

    LLVM_DEBUG(llvm::dbgs() << "==> Extract callee port name\n";);
    if (callee_port_me_expr_) {
      // callee_port_me_expr_->dump();
      callee_port_name_ =
          callee_port_me_expr_->getMemberNameInfo().getAsString();
      LLVM_DEBUG(
      llvm::dbgs() << " **** callee_port_name_: " << callee_port_name_ << "\n";
      );
    }

    // Callee is an array
    callee_array_expr_ = dyn_cast<clang::ArraySubscriptExpr>(callee_expr);
    if (callee_array_expr_) {
      LLVM_DEBUG(llvm::dbgs() << "extract callee name\n";);
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
