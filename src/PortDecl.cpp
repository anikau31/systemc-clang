#include "FindTemplateTypes.h"
#include "PortDecl.h"

using namespace scpar;
using namespace std;

PortDecl::~PortDecl() {
  // Only delete the template_type_
  if (template_type_ != nullptr) {
    delete template_type_;
    template_type_ = nullptr;
  }
}

PortDecl::PortDecl()
    : port_name_{"NONE"}, template_type_{nullptr}, field_decl_{nullptr} {}

PortDecl::PortDecl(const string &name, FindTemplateTypes *tt)
    : port_name_{name}, template_type_{tt} {}

PortDecl::PortDecl(const string &name, const FieldDecl *fd,
                   FindTemplateTypes *tt)
    : port_name_{name},
      template_type_{tt},
      field_decl_{const_cast<FieldDecl *>(fd)} {}

PortDecl::PortDecl(const PortDecl &from) {
  port_name_ = from.port_name_;
  // This is necessary to allow FindPorts to go out of scope.
  template_type_ = new FindTemplateTypes{*from.template_type_};
}

void PortDecl::setModuleName(const string &name) { port_name_ = name; }

string PortDecl::getName() const { return port_name_; }

FieldDecl *PortDecl::getFieldDecl() const { return field_decl_; }

FindTemplateTypes *PortDecl::getTemplateType() { return template_type_; }

void PortDecl::dump(llvm::raw_ostream &os, int tabn) {
  // os << "Port name: " << port_name_ << " ";
  // template_type_->printTemplateArguments(os);

  // dump_json();
}

json PortDecl::dump_json(raw_ostream &os) {
  json port_j;
  port_j["port_name"] = getName();

  auto args{template_type_->getTemplateArgTreePtr()};
  //llvm::outs() << "### Memory leak: " << args->size() << "\n";
  //args->dump();

  for (auto const &node : *args) {
    // Returns a TreeNodePtr
    auto type_data{node->getDataPtr()};
    auto parent_node{node->getParent()};
    auto parent_data{parent_node->getDataPtr()};
    if (parent_node->getDataPtr() == node->getDataPtr()) {
      //llvm::outs() << "Insert parent node: " << type_data->getTypeName()
       //            << "\n";
      port_j["port_arguments"][type_data->getTypeName()] = nullptr;
    } else {
      // FIXME: This does not print the tree properly.
      // There does not seem to be a simple way to access the appropriate
      // location for the insertion of the new values in this JSON.
      // TODO: Perhaps the way to do this is to construct a string that JSON can
      // use.
      port_j["port_arguments"][parent_data->getTypeName()].push_back(
          type_data->getTypeName());
    }
  }
  os << port_j.dump(4);
  return port_j;
}
