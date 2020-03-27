#ifndef _PORT_BINDING_H_
#define _PORT_BINDING_H_

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
}; // namespace sc_par
#endif //ifdef
