#include "EventContainer.h"
#include "clang/AST/DeclCXX.h"

using namespace systemc_clang;

EventContainer::~EventContainer() {
  // Do not delete the ast_node_.
  // Clang should take care of that.
}

EventContainer::EventContainer() : event_name_{"NONE"}, ast_node_{nullptr} {}

EventContainer::EventContainer(std::string pname, clang::VarDecl *e)
    : event_name_{pname}, ast_node_{e} {}

EventContainer::EventContainer(const EventContainer &from) {
  event_name_ = from.event_name_;
  ast_node_ = from.ast_node_;
}

std::string EventContainer::getEventName() const { return event_name_; }

clang::VarDecl *EventContainer::getASTNode() const { return ast_node_; }

void EventContainer::dump(llvm::raw_ostream &os, int tabn) {
  for (int i = 0; i < tabn; i++) {
    os << " ";
  }

  os << " EventContainer '" << event_name_ << "'\n";
}
