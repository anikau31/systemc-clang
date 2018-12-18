#include "NotifyCalls.h"

using namespace scpar;
using namespace std;

NotifyCalls::~NotifyCalls() {

	// _astNode does *NOT* need to be deleted because clang should
	// be responsible for freeing the memory.
}

NotifyCalls::NotifyCalls() :
  name_{"NONE"} {
}

NotifyCalls::NotifyCalls(const string &name, FindNotify::NotifyCallListType notify_list ) :
  name_{name},
  notify_call_list_{notify_list} {
  }

NotifyCalls::NotifyCalls( const NotifyCalls &from ) {
	name_ = from.name_;
	notify_call_list_ = from.notify_call_list_;
}

string NotifyCalls::getName() const {
	return name_;
}

FindNotify::NotifyCallListType NotifyCalls::getNotifyCallList() {
	return notify_call_list_;
}

unsigned int NotifyCalls::getNumNotifyCalls() {
	return notify_call_list_.size();
}

void NotifyCalls::dump( llvm::raw_ostream &os, int tabn ) {
	for ( size_t i{0}; i < notify_call_list_.size(); ++i ) {
    os << "\n     Notify Call: " << notify_call_list_.at(i);
  }
}
