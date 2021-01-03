#include "clang/AST/DeclCXX.h"
#include "llvm/Support/Debug.h"

#include "SignalDecl.h"
#include "FindTemplateTypes.h"

using namespace systemc_clang;

SignalDecl::~SignalDecl() {
  DEBUG_WITH_TYPE("DebugDestructors", llvm::dbgs() << "~SignalDecl\n";);
}

SignalDecl::SignalDecl() : PortDecl{} {}

SignalDecl::SignalDecl(const std::string &name, clang::FieldDecl *fd,
                       FindTemplateTypes *tt)
    : PortDecl{name, fd, tt} {}

SignalDecl::SignalDecl(const PortDecl &pd) : PortDecl{pd} {}

std::string SignalDecl::getName() { return PortDecl::getName(); }

FindTemplateTypes *SignalDecl::getTemplateTypes() {
  return PortDecl::getTemplateType();
}

const clang::FieldDecl *SignalDecl::getASTNode() const {
  return PortDecl::getAsFieldDecl();
}

json SignalDecl::dump_json() {
  json signal_j;
  signal_j["signal_name"] = getName();

  signal_j = PortDecl::dump_json();

  return signal_j;
}
