#ifndef _MODULE_INSTANCE_TYPE_
#define _MODULE_INSTANCE_TYPE_

#include <string>
#include "llvm/Support/raw_ostream.h"

namespace clang { class Decl; }

namespace sc_ast_matchers {
struct ModuleInstanceType {
  std::string var_name;
  std::string var_type_name;
  std::string instance_name;
  bool is_field_decl;
  clang::Decl *decl;
  clang::Decl *instance_decl;

  ModuleInstanceType()
      : var_name{},
        var_type_name{},
        instance_name{},
        is_field_decl{false},
        decl{nullptr} {}

  ModuleInstanceType(const ModuleInstanceType &rhs) {
    var_name = rhs.var_name;
    var_type_name = rhs.var_type_name;
    instance_name = rhs.instance_name;
    is_field_decl = rhs.is_field_decl;
    decl = rhs.decl;
    instance_decl = rhs.instance_decl;
  }

  bool operator==(const ModuleInstanceType &rhs) {
    return std::tie(var_name, var_type_name, instance_name, is_field_decl, decl,
                    instance_decl) ==
           std::tie(rhs.var_name, rhs.var_type_name, rhs.instance_name,
                    rhs.is_field_decl, rhs.decl, rhs.instance_decl);
  }

  void dump() {
    llvm::outs() << "type_decl: " << decl << " inst_decl: " << instance_decl
                 << " var_type_name: " << var_type_name
                 << " var_name: " << var_name
                 << " instance_name: " << instance_name
                 << " is_field_decl: " << is_field_decl << "\n";
  }
};
};


#endif

