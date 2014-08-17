#include <string>
#include "WaitCalls.h"

using namespace scpar;
using namespace std;

WaitCalls::~WaitCalls()
{

  // _astNode does *NOT* need to be deleted because clang should
  // be responsible for freeing the memory.
}

WaitCalls::WaitCalls():
_name("NONE")
{

}

WaitCalls::WaitCalls(const string & name,
                     FindWait::waitListType waitList):_name(name),
_waitList(waitList)
{

}

WaitCalls::WaitCalls(const WaitCalls & from)
{
  _name = from._name;
  _waitList = from._waitList;

}

string WaitCalls::getName()
{
  return _name;
}

FindWait::waitListType WaitCalls::getWaitList()
{
  return _waitList;
}

/*
FindWait::processWaitEventMapType WaitCalls::getProcessWaitEventMap() {
	return _processEventMap;
}
*/

void WaitCalls::dump(raw_ostream & os, int tabn)
{
  for (unsigned int i = 0; i < _waitList.size(); i++) {
    os << "\n     Wait Call: " << _waitList.at(i);
  }
  os << "\n     Event(s) this process is waiting on: ";

}
