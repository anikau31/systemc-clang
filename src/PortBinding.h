#ifndef _PORT_BINDING_H_
#define _PORT_BINDING_H_

#include "llvm/Support/raw_ostream.h"

using namespace clang;
using namespace systemc_clang;

namespace systemc_clang {

class PortBindingCtor {};

class PortBinding {
 private:
  // Which of the two bindings is done (sc_main or ctor)
  //
  bool is_ctor_binding_;

  //
  // These private members are used when the port binding is done in the
  // constructor.
  //
  MemberExpr *me_ctor_port_;  // port
  MemberExpr *me_instance_;   // instance on which the binding is invoked
  MemberExpr *me_arg_;        // argument to the port

  // The below private members are used when the port binding is found in the
  // sc_main.
  // There are only three pointers set, and rest are generated.
  // 1. port_member_expr_
  // 2. port_dref_
  // 3. port_parameter_dref_
  //
  MemberExpr *port_member_expr_;
  // Instance information
  std::string port_name_;
  std::string port_type_name_;

  std::string instance_type_;
  std::string instance_var_name_;
  std::string instance_constructor_name_;
  // Declaration for the instance's type.
  CXXRecordDecl *instance_type_decl_;

  // There are only two DeclRefExpr that you really need.
  // The rest will be generated from these.
  //
  // Declaration for the instance (FieldDecl/VarDecl)
  Decl *instance_decl_;
  DeclRefExpr *port_dref_;

  // Declaration for the argument for the port.
  std::string port_parameter_name_;
  const Type *port_parameter_type_;
  std::string port_parameter_type_name_;
  std::string port_parameter_bound_to_var_name_;
  DeclRefExpr *port_parameter_dref_;

 public:
  void setInstanceVarName(const std::string &n) { instance_var_name_ = n; }
  void setInstanceConstructorName(const std::string &n) {
    instance_constructor_name_ = n;
  }

  const std::string &getPortName() const { return port_name_; }
  MemberExpr *getPortMemberExpr() const { return port_member_expr_; }
  const std::string &getInstanceType() const { return instance_type_; }
  const std::string &getInstanceVarName() const { return instance_var_name_; }
  const std::string &getInstanceConstructorName() const {
    return instance_constructor_name_;
  }
  CXXRecordDecl *getInstanceTypeDecl() const { return instance_type_decl_; }
  Decl *getInstanceDecl() const { return instance_type_decl_; }
  DeclRefExpr *getPortDeclRefExpr() const { return port_dref_; }

  const std::string &getBoundToName() const { return port_parameter_name_; }
  const std::string &getBoundToParameterVarName() const {
    return port_parameter_bound_to_var_name_;
  }
  DeclRefExpr *getBoundPortDeclRefExpr() const { return port_parameter_dref_; }
  const std::string toString() const {
    return getInstanceType() + " " + getInstanceVarName() + " " +
           getInstanceConstructorName() + " " + getBoundToName();
  }

  void dump() {
    llvm::outs() << "> inst type name: " << instance_type_
                 << ", inst var name : " << instance_var_name_
                 << ", port_type name: " << port_type_name_
                 << ", port name     : " << port_name_
                 << ", port arg type : " << port_parameter_type_name_
                 << ", port_bound_to_var_name : "
                 << port_parameter_bound_to_var_name_
                 << ", port arg      : " << port_parameter_name_ << "\n";

    // llvm::outs() << "> port_name: " << port_name_ << " type: " <<
    // instance_type_
    // << " var_name: " << instance_var_name_
    // << " constructor_name: " << instance_constructor_name_
    // << " bound to " << port_parameter_name_ << "\n";
  }

  PortBinding(MemberExpr *me_ctor_port, MemberExpr *me_instance,
              MemberExpr *me_arg)
      : me_ctor_port_{me_ctor_port},
        me_instance_{me_instance},
        me_arg_{me_arg} {
    port_name_ = me_ctor_port_->getMemberNameInfo().getAsString();
    port_type_name_ = me_ctor_port->getMemberDecl()
                          ->getType()
                          .getBaseTypeIdentifier()
                          ->getName();

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
      if (MemberExpr * member_expr_kid{dyn_cast<MemberExpr>(kid)}) {
        port_parameter_bound_to_var_name_ =
            member_expr_kid->getMemberDecl()->getNameAsString();
      }
    }
  }

  // This is used for sc_main
  PortBinding(MemberExpr *me, DeclRefExpr *port_dref,
              DeclRefExpr *port_arg_dref, Decl *instance_decl,
              const std::string &instance_constructor_name)
      : is_ctor_binding_{false},
        // Used only for ctor bindings
        me_ctor_port_{nullptr},
        me_instance_{nullptr},
        me_arg_{nullptr},
        // Used only for sc_main bindings
        port_dref_{port_dref},
        port_parameter_dref_{port_arg_dref},
        instance_decl_{instance_decl},
        instance_constructor_name_{instance_constructor_name} {
    // Get the type of the instance (sc_module class name).

    port_name_ = me->getMemberDecl()->getNameAsString();
    port_type_name_ =
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

  /*
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
        */
};      // namespace systemc_clang
};      // namespace systemc_clang
#endif  // ifdef
