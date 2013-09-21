#include "PortBindContainer.h"

using namespace scpar;
using namespace std;

PortBindContainer::~PortBindContainer() {
  // Do not delete the _astNode.
  // Clang should take care of that.
}

PortBindContainer::PortBindContainer()
: _portName("NONE")
, _moduleInstanceName("NONE")
, _signalName("NONE")
, _astNode(NULL) {

}

PortBindContainer::PortBindContainer(string pname, string mname, string sname, CXXOperatorCallExpr* e)
: _portName(pname)
, _moduleInstanceName(mname)
, _signalName(sname)
,_astNode(e) {

}

PortBindContainer::PortBindContainer(const PortBindContainer& from) {
  _portName = from._portName;
  _moduleInstanceName = from._moduleInstanceName;
  _signalName = from._signalName;
  _astNode = from._astNode;
}


string PortBindContainer::getPortName() {
  return _portName;
}

string PortBindContainer::getModuleInstanceName() {
  return _moduleInstanceName;
}

string PortBindContainer::getSignalName() {
  return _signalName;
}

void PortBindContainer::dump(raw_ostream& os, int tabn) {
  for (int i =0; i < tabn; i++) 
    os << " ";

  os << " PortBindContainer '" << _portName << "' module '" << _moduleInstanceName << "' signal '" << _signalName << "'\n";
}
