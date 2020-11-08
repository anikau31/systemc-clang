#ifndef _MODULE_INSTANCE_TYPE_
#define _MODULE_INSTANCE_TYPE_

#include <string>

#include "GetASTInfo.h"

namespace clang {
class Decl;
}

#undef DEBUG_TYPE
#define DEBUG_TYPE "Dump"

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
  clang::ValueDecl *parent_decl;
  bool is_array;
  // Arrays can have multiple names.
  std::vector<std::string> instance_names;

  void add_instance_name( const std::string &name ) { instance_names.push_back(name); }
  std::vector<std::string> getInstanceNames() { return instance_names; }

  // Array fields
  bool is_array_;
  std::vector<llvm::APInt> array_sizes_;


  clang::Decl *getInstanceTypeDecl() const { return decl; }
  clang::Decl *getInstanceDecl() const { return instance_decl; }
  clang::ValueDecl *getParentDecl() const { return parent_decl; }

  /// Array handling
  void setArrayType() { is_array_ = true; }
  bool isArrayType() { return is_array_; }

  void addArraySizes(std::vector<llvm::APInt> sizes) { 
    array_sizes_ = sizes;
  }
  std::vector<llvm::APInt> getArraySizes() { return array_sizes_; }

  ModuleInstanceType()
      : var_name{},
        var_type_name{},
        instance_name{},
        parent_name{},
        is_field_decl{false},
        decl{nullptr},
        instance_decl{nullptr},
        parent_decl{nullptr},
        is_array_{false} {}

  ModuleInstanceType(const ModuleInstanceType &rhs) {
    var_name = rhs.var_name;
    var_type_name = rhs.var_type_name;
    instance_name = rhs.instance_name;
    parent_name = rhs.parent_name;
    is_field_decl = rhs.is_field_decl;
    decl = rhs.decl;
    instance_decl = rhs.instance_decl;
    parent_decl = rhs.parent_decl;
    is_array_ = rhs.is_array_;
    array_sizes_ = rhs.array_sizes_;
    instance_names = rhs.instance_names;
  }

  bool operator==(const ModuleInstanceType &rhs) {
    return std::tie(var_name, var_type_name, instance_name, parent_name,
                    is_field_decl, decl, instance_decl, parent_decl, is_array_, array_sizes_, instance_names) ==
           std::tie(rhs.var_name, rhs.var_type_name, rhs.instance_name,
                    rhs.parent_name, rhs.is_field_decl, rhs.decl,
                    rhs.instance_decl, rhs.parent_decl, rhs.is_array_, rhs.array_sizes_, instance_names);
  }

  void dump() {
    llvm::outs() << "ModuleInstanceMap --  type_decl: " << decl
                            << " inst_decl: " << instance_decl
                            << " var_type_name: " << var_type_name
                            << " var_name: " << var_name << " instance_name: "
                            << instance_name << " parent_name: " << parent_name
                            << " parent_decl: " << parent_decl
                            << " is_field_decl: " << is_field_decl 
                            << " is_array_: " << is_array_ << "\n";

    llvm::outs() << "Array sizes: " << array_sizes_.size() << " -- ";
    for (auto const &size: array_sizes_) {
      llvm::outs() << size << "  ";
    }
    
    if (instance_names.size() > 0 )  {
      llvm::outs() << "instance_names: ";
    }

    for (const auto &name: instance_names) {
      llvm::outs() << name;
    }
      llvm::outs() << "\n";
  }
};
};  // namespace sc_ast_matchers

#endif
