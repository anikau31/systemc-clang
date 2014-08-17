#include "EventContainer.h"

using namespace scpar;
using namespace std;

EventContainer::~EventContainer ()
{
	// Do not delete the _astNode.
	// Clang should take care of that.
}

EventContainer::EventContainer ():_eventName ("NONE"), _astNode (NULL)
{

}

EventContainer::EventContainer (string pname, VarDecl * e):_eventName (pname),
_astNode (e)
{

}

EventContainer::EventContainer (const EventContainer & from
)
{
	_eventName = from._eventName;
	_astNode = from._astNode;
}


string EventContainer::getEventName ()
{
	return _eventName;
}

void
  EventContainer::dump (raw_ostream & os, int tabn
)
{
	for (int i = 0; i < tabn; i++)
		os << " ";

	os << " EventContainer '" << _eventName << "'\n";
}
