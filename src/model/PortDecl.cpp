#include "clang/AST/DeclCXX.h"

#include "FindTemplateTypes.h"
#include "PortDecl.h"

using namespace scpar;

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
    : port_name_{"NONE"}, template_type_{nullptr}, field_decl_{nullptr} {}

PortDecl::PortDecl(const std::string &name, FindTemplateTypes *tt)
    : port_name_{name}, template_type_{tt}, field_decl_{nullptr} {}

PortDecl::PortDecl(const std::string &name, const Decl *fd,
                   FindTemplateTypes *tt)
    : port_name_{name},
      template_type_{tt},
      field_decl_{const_cast<Decl *>(fd)} {}

PortDecl::PortDecl(const PortDecl &from) {
  port_name_ = from.port_name_;
  /// This is necessary to allow FindPorts to go out of scope.
  template_type_ = new FindTemplateTypes{*from.template_type_};
  field_decl_ = from.field_decl_;
}

void PortDecl::setModuleName(const std::string &name) { port_name_ = name; }

std::string PortDecl::getName() const { return port_name_; }

FieldDecl *PortDecl::getFieldDecl() const {
  return dyn_cast<FieldDecl>(field_decl_);
}

VarDecl *PortDecl::getAsVarDecl() const {
  return dyn_cast<VarDecl>(field_decl_);
}
FindTemplateTypes *PortDecl::getTemplateType() { return template_type_; }

json PortDecl::dump_json() {
  json port_j;
  port_j["port_name"] = getName();
  port_j["port_arguments"] = template_type_->dump_json();

  return port_j;
}
