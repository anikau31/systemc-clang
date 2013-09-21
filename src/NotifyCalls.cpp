#include <string>
#include "NotifyCalls.h"

using namespace scpar;
using namespace std;

NotifyCalls::~NotifyCalls() {

  // _astNode does *NOT* need to be deleted because clang should
  // be responsible for freeing the memory.
}

NotifyCalls::NotifyCalls():
  _name("NONE") {

}

NotifyCalls::NotifyCalls(const string& name, FindNotify::notifyCallListType notifyCallList, FindNotify::processNotifyEventMapType processNotifyEventMap) :
  _name(name),_notifyCallList(notifyCallList), _processNotifyEventMap(processNotifyEventMap)
{

}

NotifyCalls::NotifyCalls(const NotifyCalls& from) {
  _name = from._name;
  _notifyCallList = from._notifyCallList;
	_processNotifyEventMap = from._processNotifyEventMap;
}

string NotifyCalls::getName() {
  return _name;
}

FindNotify::notifyCallListType NotifyCalls::getNotifyCallList() {
	return _notifyCallList;
}

FindNotify::processNotifyEventMapType NotifyCalls::getProcessNotifyEventMap() {
	return _processNotifyEventMap;
}

unsigned int NotifyCalls::getNumNotifyCalls() {
	return _notifyCallList.size();
}

void NotifyCalls::dump(raw_ostream& os, int tabn) {
	for (unsigned int i = 0; i<_notifyCallList.size();i++) {
		os <<"\n     Notify Call: " <<_notifyCallList.at(i);
	}
	os <<"\n     Event(s) notified by this process : " ;
	for (FindNotify::processNotifyEventMapType::iterator it = _processNotifyEventMap.begin(),eit = _processNotifyEventMap.end();
			it != eit;
			it++) {
			vector<string> tmp = it->second;
			for (int i = 0 ; i<tmp.size(); i++) {
				os <<tmp.at(i)<<" ";
			}
		}
}


