#include "clang/AST/DeclCXX.h"

#include "FindTemplateTypes.h"
#include "PortDecl.h"
#include "llvm/Support/Debug.h"

#include "APIntUtils.h"

using namespace systemc_clang;

PortDecl::~PortDecl() {
  // Only delete the template_type_
  DEBUG_WITH_TYPE("DebugDestructors", llvm::dbgs() << "~PortDecl\n";);

  if (template_type_ != nullptr) {
    delete template_type_;
    template_type_ = nullptr;
  }
  field_decl_ = nullptr;
}

PortDecl::PortDecl()
    : port_name_{"NONE"},
      template_type_{nullptr},
      field_decl_{nullptr},
      is_array_{false} {}

PortDecl::PortDecl(const std::string &name, FindTemplateTypes *tt)
    : port_name_{name},
      template_type_{tt},
      field_decl_{nullptr},
      is_array_{false} {}

PortDecl::PortDecl(const std::string &name, const clang::Decl *fd,
                   FindTemplateTypes *tt)
    : port_name_{name},
      template_type_{tt},
      field_decl_{const_cast<clang::Decl *>(fd)},
      is_array_{false} {}

PortDecl::PortDecl(const PortDecl &from) {
  port_name_ = from.port_name_;
  /// This is necessary to allow FindPorts to go out of scope.
  template_type_ = new FindTemplateTypes{*from.template_type_};
  field_decl_ = from.field_decl_;
  is_array_ = from.is_array_;
  array_sizes_ = from.array_sizes_;
}

void PortDecl::addArraySize(llvm::APInt sz) { array_sizes_.push_back(sz); }

void PortDecl::setArrayType() { is_array_ = true; }

bool PortDecl::getArrayType() const { return is_array_; }

bool PortDecl::isPointerType() const {
  if (const clang::FieldDecl *fd = getAsFieldDecl()) {
    if (auto ptr_type = fd->getType().getTypePtr()) {
      return ptr_type->isPointerType();
    }
  }

  if (const clang::VarDecl *vd = getAsVarDecl()) {
    if (auto ptr_type = vd->getType().getTypePtr()) {
      return ptr_type->isPointerType();
    }
  }
  return false;
}

void PortDecl::setModuleName(const std::string &name) { port_name_ = name; }

std::vector<llvm::APInt> PortDecl::getArraySizes() const {
  return array_sizes_;
}

std::string PortDecl::getName() const { return port_name_; }

clang::FieldDecl *PortDecl::getAsFieldDecl() const {
  return clang::dyn_cast<clang::FieldDecl>(field_decl_);
}

clang::VarDecl *PortDecl::getAsVarDecl() const {
  return clang::dyn_cast<clang::VarDecl>(field_decl_);
}

FindTemplateTypes *PortDecl::getTemplateType() { return template_type_; }

std::string PortDecl::asString() const {
  std::string str{};
  std::string class_name{};
  if (auto fd = getAsFieldDecl()) {
    class_name = fd->getParent()->getName().str();
  }

  str += "signal_port_name: " + class_name + "::" + getName() + "\n";
  str += "signal_port_arguments: " + template_type_->asString() + "\n";
  if (getArrayType()) {
    str += "is_array_type: true\n";
  }
  else {
    str += "is_array_type: false\n";
  }
  if (isPointerType()) {
    str += "is_pointer_type: true \n";
  } else {
    str += "is_pointer_type: false\n";
  }

  if (getArrayType()) {
    str += "array_sizes: ";
    for (const auto sz : getArraySizes()) {
      std::size_t i{0};
      // std::string s = systemc_clang::utils::apint::toString(sz);

      str += systemc_clang::utils::apint::toString(sz);
    }
  }
  str += "\n";

  if (getAsFieldDecl()) {
    str += "decl_type: FieldDecl";
  } else {
    if (getAsVarDecl()) {
      str += "decl_type: VarDecl";
    }
  }

  str += "\n";
  return str;
}
