#ifndef _PORT_BINDING_H_
#define _PORT_BINDING_H_

#include "llvm/Support/raw_ostream.h"

using namespace clang;
using namespace scpar;

namespace scpar {

class PortBinding {
 private:
  std::string port_name_;
  MemberExpr *port_member_expr_;
  // Instance information
  std::string instance_type_;
  std::string instance_var_name_;
  std::string instance_constructor_name_;
  // Declaration for the instance's type.
  CXXRecordDecl *instance_type_decl_;

  // Declaration for the instance (FieldDecl/VarDecl)
  Decl *instance_decl_;
  DeclRefExpr *port_dref_;

  std::string port_parameter_name_;
  DeclRefExpr *port_parameter_dref_;

 public:
  const std::string &getPortName() const { return port_name_; }
  MemberExpr *getPortMemberExpr() const { return port_member_expr_; }
  const std::string &getInstanceType() const { return instance_type_; }
  const std::string &getInstanceVarName() const { return instance_var_name_; }
  const std::string &getInstanceConstructorName() const {
    return instance_constructor_name_;
  }
  CXXRecordDecl *getInstanceTypeDecl() const { return instance_type_decl_;}
  Decl* getInstanceDecl() const { return instance_type_decl_;}
  DeclRefExpr *getPortDeclRefExpr() const { return port_dref_; }

  const std::string &getBoundToName() const { return port_parameter_name_; }
  DeclRefExpr *getBoundPortDeclRefExpr() const { return port_parameter_dref_;}
  const std::string toString() const {
    return getInstanceType() + " " + getInstanceVarName() + " " +
           getInstanceConstructorName() + " " + getBoundToName();
  }

  void dump() {
    llvm::outs() << "> port_name: " << port_name_ << " type: " << instance_type_
                 << " var_name: " << instance_var_name_
                 << " constructor_name: " << instance_constructor_name_
                 << " bound to " << port_parameter_name_ << "\n";
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
};      // namespace scpar
#endif  // ifdef
