#include "EventDecl.h"
#include "FindTemplateTypes.h"

using namespace systemc_clang;

EventDecl::~EventDecl() {
  // _astNode does *NOT* need to be deleted because clang should
  // be responsible for freeing the memory.
}

EventDecl::EventDecl() : name_{"NONE"}, ast_node_{nullptr} {}

EventDecl::EventDecl(const std::string &name, clang::FieldDecl *fd)
    : name_{name}, ast_node_{fd} {}

EventDecl::EventDecl(const EventDecl &from) {
  name_ = from.name_;
  ast_node_ = from.ast_node_;
}

std::string EventDecl::getName() const { return name_; }

const clang::FieldDecl *EventDecl::getASTNode() const { return ast_node_; }

void EventDecl::dump(llvm::raw_ostream &os) {
  os << "EventDecl " << this << "  " << name_ << " FieldDecl " << getASTNode();
}
