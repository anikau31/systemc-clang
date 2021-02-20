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

std::string SignalDecl::getName() const { return PortDecl::getName(); }

FindTemplateTypes *SignalDecl::getTemplateTypes() {
  return PortDecl::getTemplateType();
}

const clang::FieldDecl *SignalDecl::getASTNode() const {
  return PortDecl::getAsFieldDecl();
}

std::string SignalDecl::asString() const {
  std::string str{};

  str += "signal_name: " + getName() + "\n";
  str += PortDecl::asString() + "\n";

  return str;
}

