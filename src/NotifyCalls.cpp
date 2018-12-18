#include <string>
#include "NotifyCalls.h"

using namespace scpar;
using namespace std;

NotifyCalls::~NotifyCalls() {

	// _astNode does *NOT* need to be deleted because clang should
	// be responsible for freeing the memory.
}

NotifyCalls::NotifyCalls() :
  _name{"NONE"} {
}

NotifyCalls::NotifyCalls(const string & name, FindNotify::NotifyCallListType notifyCallList ) :
  _name{name},
  _notifyCallList{notifyCallList} {

}

NotifyCalls::NotifyCalls( const NotifyCalls & from ) {
	_name = from._name;
	_notifyCallList = from._notifyCallList;
}

string NotifyCalls::getName() const {
	return _name;
}

FindNotify::NotifyCallListType NotifyCalls::getNotifyCallList() {
	return _notifyCallList;
}


unsigned int NotifyCalls::getNumNotifyCalls() {
	return _notifyCallList.size ();
}

void NotifyCalls::dump ( llvm::raw_ostream & os, int tabn ) {
	for ( auto i{0}; i < _notifyCallList.size (); ++i ) {
    os << "\n     Notify Call: " << _notifyCallList.at (i);
  }
}
