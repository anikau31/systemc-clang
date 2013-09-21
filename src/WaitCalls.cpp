#include <string>
#include "WaitCalls.h"

using namespace scpar;
using namespace std;

WaitCalls::~WaitCalls() {

  // _astNode does *NOT* need to be deleted because clang should
  // be responsible for freeing the memory.
}

WaitCalls::WaitCalls():
  _name("NONE") {

}

WaitCalls::WaitCalls(const string& name, FindWait::waitListType waitList, 
FindWait::waitTimesMapType waitTimesMap, FindWait::processWaitEventMapType 
processEventMap) :
  _name(name),
	_waitList(waitList),
	_waitTimesMap(waitTimesMap),
	_processEventMap(processEventMap)
{

}

WaitCalls::WaitCalls(const WaitCalls& from) {
  _name = from._name;
  _waitList = from._waitList;
  _waitTimesMap = from._waitTimesMap;
	_processEventMap = from._processEventMap;
}

string WaitCalls::getName() {
  return _name;
}

FindWait::waitListType WaitCalls::getWaitList() {
  return _waitList;
}

FindWait::waitTimesMapType WaitCalls::getWaitTimes() {
  return _waitTimesMap;
}

FindWait::processWaitEventMapType WaitCalls::getProcessWaitEventMap() {
	return _processEventMap;
}

unsigned int WaitCalls::getTotalWaits() {
	unsigned int count = 0;

	for (unsigned int i = 0; i<_waitList.size();i++) {
		count++;
		if(_waitTimesMap.find(_waitList.at(i)) != _waitTimesMap.end()) {
				FindWait::waitTimesMapType::iterator callFound = _waitTimesMap.find(_waitList.at(i));
				count = count + (callFound->second-1);
		}
	}
	return count;
}

/*
unsigned int WaitCalls::getSizeMember() {
	unsigned int size = 0;
	for (FindWait::waitVariablesMapType::iterator it = _waitVariableMap.begin(), eit = _waitVariableMap.end();
				it != eit;
				it++) {
			vector<string> tmp = it->second;
			for (unsigned int i = 0; i<tmp.size();i++) {
				if(_classMemberSizeMap.find(tmp.at(i)) != _classMemberSizeMap.end()) {
					FindClassDataMembers::classMemberSizeMapType::iterator memberFound = _classMemberSizeMap.find(tmp.at(i));
					vector<unsigned int> memberSize = memberFound->second;	
					unsigned int tmpSize = 0;
					for (unsigned int j = 0; j<memberSize.size();j++) {
						tmpSize = tmpSize + memberSize.at(j);
					}
					size = size + tmpSize;
				}
				else {
					size = size + 1;
				}
			}
		}	
	return size;
}
*/

void WaitCalls::dump(raw_ostream& os, int tabn) {
	for (unsigned int i = 0; i<_waitList.size();i++) {
		os <<"\n     Wait Call: " <<_waitList.at(i);
		if(_waitTimesMap.find(_waitList.at(i)) != _waitTimesMap.end()) {
				FindWait::waitTimesMapType::iterator callFound = _waitTimesMap.find(_waitList.at(i));
				os <<" Number of times called : " <<callFound->second; 
		}	
	}
	os <<"\n     Event(s) this process is waiting on: ";
	for (FindWait::processWaitEventMapType::iterator it = _processEventMap.begin(), eit = _processEventMap.end(); 
				it != eit;
				it++) {
				vector<string> tmp = it->second;
				for (int i = 0; i<tmp.size(); i++) {
					os <<tmp.at(i)<<" ";	
				}		
		}
}
