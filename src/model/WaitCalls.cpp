#include "WaitCalls.h"

#include "clang/AST/DeclCXX.h"


using namespace systemc_clang;

WaitCalls::~WaitCalls() {
  // _astNode does *NOT* need to be deleted because clang should
  // be responsible for freeing the memory.
}

WaitCalls::WaitCalls() : _name("NONE") {}

WaitCalls::WaitCalls(const std::string &name, FindWait::waitListType waitList)
    : _name(name), _waitList(waitList) {}

WaitCalls::WaitCalls(const WaitCalls &from) {
  _name = from._name;
  _waitList = from._waitList;
}

std::string WaitCalls::getName() { return _name; }

FindWait::waitListType WaitCalls::getWaitList() { return _waitList; }

void WaitCalls::dump(llvm::raw_ostream &os, int tabn) {
  for (unsigned int i = 0; i < _waitList.size(); i++) {
    os << "\n     Wait Call: " << _waitList.at(i);
  }
  os << "\n     Event(s) this process is waiting on: ";
}
