#ifndef _MODULE_INSTANCE_TYPE_
#define _MODULE_INSTANCE_TYPE_

#include <string>
#include "llvm/Support/raw_ostream.h"

namespace clang {
class Decl;
}

namespace sc_ast_matchers {
struct ModuleInstanceType {
  std::string var_name;
  std::string var_type_name;
  std::string instance_name;
  std::string
      parent_name;  // This is the name of the class that holds the field.
  bool is_field_decl;
  clang::Decl *decl;
  clang::Decl *instance_decl;
  clang::RecordDecl *parent_decl;

  ModuleInstanceType()
      : var_name{},
        var_type_name{},
        instance_name{},
        parent_name{},
        is_field_decl{false},
        decl{nullptr},
        instance_decl{nullptr},
        parent_decl{nullptr} {}

  ModuleInstanceType(const ModuleInstanceType &rhs) {
    var_name = rhs.var_name;
    var_type_name = rhs.var_type_name;
    instance_name = rhs.instance_name;
    parent_name = rhs.parent_name;
    is_field_decl = rhs.is_field_decl;
    decl = rhs.decl;
    instance_decl = rhs.instance_decl;
    parent_decl = rhs.parent_decl;
  }

  bool operator==(const ModuleInstanceType &rhs) {
    return std::tie(var_name, var_type_name, instance_name, parent_name,
                    is_field_decl, decl, instance_decl, parent_decl) ==
           std::tie(rhs.var_name, rhs.var_type_name, rhs.instance_name,
                    rhs.parent_name, rhs.is_field_decl, rhs.decl,
                    rhs.instance_decl, rhs.parent_decl);
  }

  void dump() {
    llvm::outs() << "type_decl: " << decl << " inst_decl: " << instance_decl
                 << " var_type_name: " << var_type_name
                 << " var_name: " << var_name
                 << " instance_name: " << instance_name
                 << " parent_name: " << parent_name
                 << " parent_decl: " << parent_decl
                 << " is_field_decl: " << is_field_decl << "\n";
  }
};
};  // namespace sc_ast_matchers

#endif
