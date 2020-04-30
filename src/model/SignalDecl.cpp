#include <string>

#include "clang/AST/DeclCXX.h"

#include "SignalDecl.h"

using namespace scpar;

SignalDecl::~SignalDecl() { llvm::outs() << "~SignalDecl\n"; }

SignalDecl::SignalDecl() : PortDecl{} {}

SignalDecl::SignalDecl(const std::string &name, clang::FieldDecl *fd,
                       FindTemplateTypes *tt)
    : PortDecl{name, fd, tt} {}

std::string SignalDecl::getName() { return PortDecl::getName(); }

FindTemplateTypes *SignalDecl::getTemplateTypes() {
  return PortDecl::getTemplateType();
}

clang::FieldDecl *SignalDecl::getASTNode() { return PortDecl::getFieldDecl(); }

json SignalDecl::dump_json(raw_ostream &os) {
  json signal_j;
  signal_j["signal_name"] = getName();

  // Container
  auto template_args{PortDecl::getTemplateType()->getTemplateArgumentsType()};

  signal_j["signal_type"] = template_args[0].getTypeName();
  template_args.erase(begin(template_args));

  for (auto ait = begin(template_args); ait != end(template_args); ++ait) {
    signal_j["signal_arguments"].push_back(ait->getTypeName());
  }

  // os << signal_j.dump(4);

  return signal_j;
}
