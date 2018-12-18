#include "EventContainer.h"

using namespace scpar;
using namespace std;

EventContainer::~EventContainer() {
	// Do not delete the ast_node_.
	// Clang should take care of that.
}

EventContainer::EventContainer():
  event_name_{"NONE"},
  ast_node_{nullptr} {

}

EventContainer::EventContainer( string pname, VarDecl * e ) :
  event_name_{pname},
  ast_node_{e} {

}

EventContainer::EventContainer( const EventContainer & from ) {
	event_name_ = from.event_name_;
	ast_node_ = from.ast_node_;
}

string EventContainer::getEventName() const {
	return event_name_;
}

VarDecl * EventContainer::getASTNode() const {
  return ast_node_;
}

void EventContainer::dump( llvm::raw_ostream & os, int tabn ) {
	for (int i = 0; i < tabn; i++) {
		os << " ";
  }

	os << " EventContainer '" << event_name_ << "'\n";
}
